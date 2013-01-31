##  Blender Export Script
##  Copyright (C) 2012 Ingo Ruhnke <grumbel@gmail.com>
##
##  This program is free software: you can redistribute it and/or modify
##  it under the terms of the GNU General Public License as published by
##  the Free Software Foundation, either version 3 of the License, or
##  (at your option) any later version.
##
##  This program is distributed in the hope that it will be useful,
##  but WITHOUT ANY WARRANTY; without even the implied warranty of
##  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
##  GNU General Public License for more details.
##
##  You should have received a copy of the GNU General Public License
##  along with this program.  If not, see <http://www.gnu.org/licenses/>.

import bpy
from mathutils import Matrix, Vector
from collections import namedtuple

def pm(mat):
    print("Matrix(%6.2f  %6.2f  %6.2f  %6.2f\n"
          "       %6.2f  %6.2f  %6.2f  %6.2f\n"
          "       %6.2f  %6.2f  %6.2f  %6.2f\n"
          "       %6.2f  %6.2f  %6.2f  %6.2f)" % 
          (mat[0][0], mat[1][0], mat[2][0], mat[3][0], 
           mat[0][1], mat[1][1], mat[2][1], mat[3][1], 
           mat[0][2], mat[1][2], mat[2][2], mat[3][2], 
           mat[0][3], mat[1][3], mat[2][3], mat[3][3]))

Face   = namedtuple('Face',   ['v1', 'v2', 'v3'])
Vertex = namedtuple('Vertex', ['co', 'n', 'uv', 'bones'])

def vec3(v):
    """Convert Blender vector into tuple, swaps Y and Z"""
    # return (v.x, v.z, -v.y)
    return (v.x, v.z, -v.y)

def write_mesh(obj):
    faces = collect_faces(obj)
    faces, vertices = index_vertices(faces)

    outfile.write("o %s\n" % obj.name)
    outfile.write("loc %f %f %f\n" % vec3(obj.location))

    if obj.rotation_mode == 'XYZ':
        w, x, y, z = obj.rotation_euler.to_quaternion()
    elif obj.rotation_mode == 'QUATERNION':
        w, x, y, z = obj.rotation_quaternion
    else:
        raise Exception("unsupported rotation mode: %s" % obj.rotation_mode)

    outfile.write("rot %f %f %f %f\n" % (w, x, z, -y))

    x, y, z = obj.scale
    outfile.write("scale %f %f %f\n" % (x, z, -y))

    print("vertices: %d" % len(vertices))
    print("faces: %d" % len(faces))

    for v in vertices:
        outfile.write("vn %f %f %f\n" % v.n)
        if v.uv:
            outfile.write("vt %f %f\n" % v.uv)

        if v.bones:
            bones = list(v.bones)

            bones.sort(key=lambda bone: bone[1], reverse=True)

            while len(bones) < 4:
                bones.append((0, 0.0))

            while len(bones) > 4:
                bones.pop()

            bone_index  = [g for g, w in bones]
            bone_weight = [w for g, w in bones]
            bone_weight = [w / sum(bone_weight) for w in bone_weight]

            outfile.write("bi %d %d %d %d\n" % tuple(bone_index))
            outfile.write("bw %f %f %f %f\n" % tuple(bone_weight))
        
        outfile.write("v %f %f %f\n" % v.co)

    for f in faces:
        outfile.write("f %d %d %d\n" % (f.v1, f.v2, f.v3))

def index_vertices(faces):
    # collect vertices
    vertices = {}
    for face in faces:
        vertices[face.v1] = None
        vertices[face.v2] = None
        vertices[face.v3] = None
        
    # number the vertices
    i = 0
    for k in vertices.keys():
        vertices[k] = i
        i += 1

    # replace vertices with index
    out_faces = []
    for face in faces:
        out_faces.append(Face(vertices[face.v1],
                              vertices[face.v2],
                              vertices[face.v3]))

    return out_faces, list(vertices.keys())

def collect_faces(obj):
    """collect data from the given mesh and triangulate it"""

    bone_name2idx = {}
    if len(obj.modifiers) == 1 and obj.modifiers[0].type == "ARMATURE":
        for i, bone in enumerate(obj.modifiers[0].object.data.bones):
            bone_name2idx[bone.name] = i

    print(bone_name2idx)

    mesh = obj.data

    uv_faces = None
    print(dir(mesh))
    mesh.update(calc_tessface=True)
    faces = mesh.tessfaces
    print("Faces: ", faces)
    if mesh.uv_textures.active:
        uv_faces = mesh.tessface_uv_textures.active.data

    out_faces = []

    # bpy.data.objects[0].data.vertices[1].groups -> VertexGroupElement
    # bpy.data.objects[0].data.vertices[1].groups[0] -> weight
        
    for face in faces:
        num_vertices = len(face.vertices)
        v = [mesh.vertices[face.vertices[i]] for i in range(0, num_vertices)]

        if uv_faces:
            uv = [tuple(uv_faces[face.index].uv[i]) for i in range(0, num_vertices)]
        else:
            uv = [None, None, None, None]

        bones = [[], [], [], []]
        for i, vert in enumerate(v):
            for j, g in enumerate(mesh.vertices[vert.index].groups):
                bones[i].append((bone_name2idx[obj.vertex_groups[g.group].name], g.weight))
            bones[i] = tuple(bones[i])
        bones = tuple(bones)

        out_faces.append(
            Face(Vertex(vec3(v[1].co), vec3(v[1].normal), uv[1], bones[1]),
                 Vertex(vec3(v[0].co), vec3(v[0].normal), uv[0], bones[0]),
                 Vertex(vec3(v[2].co), vec3(v[2].normal), uv[2], bones[2])))
       
        if num_vertices == 4:
            out_faces.append(
                Face(Vertex(vec3(v[2].co), vec3(v[2].normal), uv[2], bones[2]),
                     Vertex(vec3(v[0].co), vec3(v[0].normal), uv[0], bones[0]),
                     Vertex(vec3(v[3].co), vec3(v[3].normal), uv[3], bones[3])))

    return out_faces

def vec3_str(v):
    return "%6.2f %6.2f %6.2f" % (v.x, v.z, -v.y)

def vec4_str(v):
    return "%6.2f %6.2f %6.2f %6.2f" % (v.x, v.z, -v.y, v.w)

def mat3_str(m):
    return "%s %s %s" % (vec3_str(m[0]), vec3_str(m[1]), vec3_str(m[2]))

def mat4_str(m):
    return "%s %s %s %s" % (vec4_str(m[0]), vec4_str(m[1]), vec4_str(m[2]), vec4_str(m[3]))

def write_armature(obj):
    with open("/tmp/blender.bones", "w") as f:
        f.write("# exported by %s\n" % __file__)
        armature = obj.data
        for bone in armature.bones:
            # _local is in armature space, the other in bone space
            f.write("bone %s\n" % bone.name)
            if bone.parent: 
                f.write("  parent       %s\n" % bone.parent.name)
            f.write("  head         %s\n" % vec3_str(bone.head))
            f.write("  tail         %s\n" % vec3_str(bone.tail))
            f.write("  head_local   %s\n" % vec3_str(bone.head_local))
            f.write("  tail_local   %s\n" % vec3_str(bone.tail_local))
            f.write("  matrix       %s\n" % mat3_str(bone.matrix)) # a 3x3 matrix
            f.write("  matrix_local %s\n" % mat4_str(bone.matrix_local)) # a 4x4 matrix
            f.write("\n")

    with open("/tmp/blender.pose", "w") as f:
        f.write("# exported by %s\n" % __file__)
        for bone in obj.pose.bones:
            f.write("bone %s\n" % bone.name)
            f.write("  matrix       %s\n" % mat4_str(bone.matrix))
            f.write("  matrix_basis %s\n" % mat4_str(bone.matrix_basis))
            f.write("\n")

# [(x.name, k) for k,v in enumerate(bpy.data.objects[0].data.bones)]
# [(v.name, v.index) for v in bpy.data.objects[1].vertex_groups]

with open("/tmp/blender.mod", "w") as outfile:
    outfile.write("# exported by %s\n" % __file__)
    meshes = [obj for obj in bpy.data.objects if obj.type == 'MESH']
    armatures = [obj for obj in bpy.data.objects if obj.type == 'ARMATURE']
    for mesh in meshes:
        write_mesh(mesh)
    for armature in armatures:
        write_armature(armature)
    outfile.write("\n# EOF #\n")

print("-- export complete --")

# EOF #
