import io
import os
import json
import mathutils
import bpy
import bmesh

bl_info = {
	"name": "Bullet",
	"description": "Exports bullet physics file.",
	"author": "xu-xionglong, biller23, jdl89",
	"blender": (2, 80, 0),
	"location": "File > Export > Bullet",
	"category": "Import-Export",
}

def writeVec3(inVec3):
	# NOTE: Y and Z have been flipped due to Blender's axes.
	return {"x": inVec3[0], "y": inVec3[2], "z": inVec3[1]}
    
def writeVec3Flipped(inVec3):
	# NOTE: Y and Z have been flipped due to Blender's axes.
	return {"x": inVec3[0], "y": inVec3[2], "z": -inVec3[1]}
    
def writeVec3Flipped2(inVec3):
	# NOTE: Y and Z have been flipped due to Blender's axes.
	return {"x": inVec3[0], "y": inVec3[2], "z": inVec3[1]}

def writeQuaternion(inQuaternion):
	conversion_matrix = mathutils.Matrix(((1, 0, 0, 0), (0, 1, 0, 0), (0, 0, 1, 0), (0, 0, 0, 1)))

	axis = mathutils.Vector((1, 0, 0))  # Example: X-axis
	angle_radians = 0#-1.5708  # -90 degrees
	quat3 = mathutils.Quaternion(axis, angle_radians)

	engine_quat = quat3 @ inQuaternion
	engine_quat.normalize()
	# NOTE: Y and Z have been flipped due to Blender's axes.
	return {"x": engine_quat[1], "y": engine_quat[3], "z": -engine_quat[2], "w": engine_quat[0]}
	
def writeTxtQuaternion(inQuaternion, buffer):
	conversion_matrix = mathutils.Matrix(((1, 0, 0, 0), (0, 1, 0, 0), (0, 0, 1, 0), (0, 0, 0, 1)))

	axis = mathutils.Vector((1, 0, 0))  # Example: X-axis
	angle_radians = 0#-1.5708  # -90 degrees
	quat3 = mathutils.Quaternion(axis, angle_radians)

	engine_quat = quat3 @ inQuaternion
	engine_quat.normalize()
	
	buffer.write("\trot: " + str(engine_quat[1]) + " " + str(engine_quat[3]) + " " + str(-engine_quat[2]) + " " + str(engine_quat[0]))


	# Below is the original code. The position and orientation of bodies seems correct, but breaks when any rotation is added.
	# return {"x": -inQuaternion[1], "y": -inQuaternion[3], "z": -inQuaternion[2], "w": inQuaternion[0]}

def getOffsetFromAToB(a, b):
	ta, ra, sa = a.matrix_world.decompose()
	tb, rb, sb = b.matrix_world.decompose()
	MtaInv = mathutils.Matrix.Translation(-ta)
	MraInv = ra.to_matrix().inverted().to_4x4()
	Mtb = mathutils.Matrix.Translation(tb)
	Mrb = rb.to_matrix().to_4x4()
	Moffset = MraInv @ MtaInv @ Mtb @ Mrb
	tOffset, rOffset, sOffset = Moffset.decompose()
	tOffset.x = tOffset.x / sa.x
	tOffset.y = tOffset.y / sa.y
	tOffset.z = tOffset.z / sa.z
	return tOffset, rOffset

def save(context, path, out_hulls, out_meshes):
	jsonObject = {}

	scene = context.scene
	jsonObject["gravity"] = scene.gravity[:]
	jsonObject["rigid_bodys"] = []
	jsonObject["constraints"] = []
	jsonObject["convex_hulls"] = []
	jsonObject["meshes"] = []
	jsonObject["tags"] = []
	
	buffer = io.StringIO()

	objects_with_rigid_body = [obj for obj in scene.objects if obj.rigid_body is not None]

	buffer.write("shapeCount:\t" + str(len(objects_with_rigid_body)) + "\n")

	for obj in objects_with_rigid_body:
		print("DUMPING INFO FOR ", obj.name)
		if obj.rigid_body is not None:
			transform = obj.matrix_world
			location, quaternion, scale = transform.decompose()
			rigidBodyObject = {}
			rigidBodyObject["name"] = obj.name
			rigidBodyObject["location"] = writeVec3Flipped(location)
			rigidBodyObject["quaternion"] = writeQuaternion(quaternion)
			rigidBodyObject["scale"] = writeVec3(scale)
			rigidBodyObject["dimensions"] = writeVec3(obj.dimensions)
			rigidBodyObject["static"] = not obj.rigid_body.enabled
			rigidBodyObject["kinematic"] = obj.rigid_body.kinematic
			rigidBodyObject["mass"] = obj.rigid_body.mass
			rigidBodyObject["friction"] = obj.rigid_body.friction
			rigidBodyObject["restitution"] = obj.rigid_body.restitution
			rigidBodyObject["collision_shape"] = obj.rigid_body.collision_shape

			buffer.write("shape:\t" + obj.rigid_body.collision_shape)
			buffer.write("\tname: " + obj.name)
			buffer.write("\tpos: " + str(obj.location[0]) + " " + str(obj.location[2]) + " " + str(-obj.location[1]))
			buffer.write("\tdim: " + str(obj.dimensions[0]) + " " + str(obj.dimensions[2]) + " " + str(-obj.dimensions[1]))
			writeTxtQuaternion(quaternion, buffer)
			buffer.write("\n")

			rigidBodyObject["attachments"] = []

			if obj.rigid_body.collision_shape == 'CONVEX_HULL':
				rigidBodyObject["hull_name"] = obj.data.name

			if obj.rigid_body.collision_shape == 'MESH':
				rigidBodyObject["mesh_name"] = obj.data.name

			rigidBodyObject["use_margin"] = obj.rigid_body.use_margin
			rigidBodyObject["collision_margin"] = obj.rigid_body.collision_margin
			group = 0

			for i in range(0, len(obj.rigid_body.collision_collections)):
				if obj.rigid_body.collision_collections[i]:
					group = group | (1 << i)
			rigidBodyObject["group"] = group
			rigidBodyObject["mask"] = group
			jsonObject["rigid_bodys"].append(rigidBodyObject)

			# export convex hull
			if out_hulls == True and obj.rigid_body.collision_shape == 'CONVEX_HULL':
				save_hull = True
				for i in jsonObject["convex_hulls"]:
					if (i["hull_name"] == obj.data.name) :
						save_hull = False
				if (save_hull == True):
					hullObject = {}
					hullObject["hull_name"] = obj.data.name
					hull = []
					for i in range(len(obj.data.vertices)):
						hull.append(writeVec3Flipped(obj.data.vertices[i].co))
					hullObject["hull_points"] = hull
					jsonObject["convex_hulls"].append(hullObject)

			# export triangle meshes
			if out_meshes == True and obj.rigid_body.collision_shape == 'MESH':
				save_mesh = True
				for i in jsonObject["meshes"]:
					if (i["mesh_name"] == obj.data.name) :
						save_mesh = False
				if (save_mesh == True):
					meshObject = {}
					meshObject["mesh_name"] = obj.data.name
					mesh_verts = []
					mesh_indices = []

					bm = bmesh.new()
					bm.from_mesh(obj.data)
					bmesh.ops.triangulate(bm, faces=bm.faces)
					bm.verts.ensure_lookup_table()
					bm.faces.ensure_lookup_table()

					for i in range(len(bm.verts)):
						mesh_verts.append(writeVec3Flipped(bm.verts[i].co))
						meshObject["mesh_verts"] = mesh_verts

					for f in range(len(bm.faces)):
						for i in range(3):
							mesh_indices.append(bm.faces[f].verts[i].index)

					meshObject["mesh_indices"] = mesh_indices
					jsonObject["meshes"].append(meshObject)
					bm.free()
					del bm
					
			for currentChild in obj.children:
				if currentChild.type == 'EMPTY' and currentChild.empty_display_type == 'ARROWS':
					attachmentObject = {}
					attachmentObject["name"] = currentChild.name
					tOffset, rOffset = getOffsetFromAToB(currentChild.parent, currentChild)
					attachmentObject["position"] = writeVec3Flipped(tOffset)
					attachmentObject["orientation"] = writeQuaternion(rOffset)
			
					rigidBodyObject["attachments"].append(attachmentObject)

		if obj.rigid_body_constraint is not None:
			rigidBodyConstraintObject = {}
			constraintType = obj.rigid_body_constraint.type
			rigidBodyConstraintObject["name"] = obj.name
			rigidBodyConstraintObject["type"] = constraintType
			rigidBodyConstraintObject["enabled"] = obj.rigid_body_constraint.enabled
			rigidBodyConstraintObject["disable_collisions"] = obj.rigid_body_constraint.disable_collisions
			rigidBodyConstraintObject["breaking_threshold"] = obj.rigid_body_constraint.breaking_threshold
			rigidBodyConstraintObject["use_breaking"] = obj.rigid_body_constraint.use_breaking #todo: replace by breaking thredhold value?
			rigidBodyConstraintObject["use_override_solver_iterations"] = obj.rigid_body_constraint.use_override_solver_iterations
			rigidBodyConstraintObject["solver_iterations"] = obj.rigid_body_constraint.solver_iterations

			transform = obj.matrix_world
			location, quaternion, scale = transform.decompose()
			rigidBodyConstraintObject["quaternion"] = writeQuaternion(quaternion)
            
			object1 = obj.rigid_body_constraint.object1
			if object1 is not None:
				rigidBodyConstraintObject["object1"] = object1.name
				tOffset, rOffset = getOffsetFromAToB(object1, obj)
				rigidBodyConstraintObject["translation_offset_a"] = writeVec3Flipped(tOffset)
				rigidBodyConstraintObject["rotation_offset_a"] = writeQuaternion(rOffset)

			object2 = obj.rigid_body_constraint.object2
			if object2 is not None:
				rigidBodyConstraintObject["object2"] = object2.name
				tOffset, rOffset = getOffsetFromAToB(object2, obj)
				rigidBodyConstraintObject["translation_offset_b"] = writeVec3Flipped(tOffset)
				rigidBodyConstraintObject["rotation_offset_b"] = writeQuaternion(rOffset)

			if constraintType == 'HINGE':
				rigidBodyConstraintObject["use_limit_ang_z"] = obj.rigid_body_constraint.use_limit_ang_z
				rigidBodyConstraintObject["limit_ang_z_lower"] = obj.rigid_body_constraint.limit_ang_z_lower
				rigidBodyConstraintObject["limit_ang_z_upper"] = obj.rigid_body_constraint.limit_ang_z_upper
			if constraintType == 'MOTOR':
				rigidBodyConstraintObject["use_motor_ang"] = obj.rigid_body_constraint.use_motor_ang
				rigidBodyConstraintObject["motor_ang_target_velocity"] = obj.rigid_body_constraint.motor_ang_target_velocity
				rigidBodyConstraintObject["motor_ang_max_impulse"] = obj.rigid_body_constraint.motor_ang_max_impulse
				rigidBodyConstraintObject["use_motor_lin"] = obj.rigid_body_constraint.use_motor_lin
				rigidBodyConstraintObject["motor_lin_target_velocity"] = obj.rigid_body_constraint.motor_lin_target_velocity
				rigidBodyConstraintObject["motor_lin_max_impulse"] = obj.rigid_body_constraint.motor_lin_max_impulse
			elif constraintType == 'SLIDER':
				rigidBodyConstraintObject["use_limit_lin_x"] = obj.rigid_body_constraint.use_limit_lin_x
				rigidBodyConstraintObject["limit_lin_x_lower"] = obj.rigid_body_constraint.limit_lin_x_lower
				rigidBodyConstraintObject["limit_lin_x_upper"] = obj.rigid_body_constraint.limit_lin_x_upper
			elif constraintType == 'PISTON':
				rigidBodyConstraintObject["use_limit_lin_x"] = obj.rigid_body_constraint.use_limit_lin_x
				rigidBodyConstraintObject["limit_lin_x_lower"] = obj.rigid_body_constraint.limit_lin_x_lower
				rigidBodyConstraintObject["limit_lin_x_upper"] = obj.rigid_body_constraint.limit_lin_x_upper
				rigidBodyConstraintObject["use_limit_ang_x"] = obj.rigid_body_constraint.use_limit_ang_x
				rigidBodyConstraintObject["limit_ang_x_lower"] = obj.rigid_body_constraint.limit_ang_x_lower
				rigidBodyConstraintObject["limit_ang_x_upper"] = obj.rigid_body_constraint.limit_ang_x_upper
			elif constraintType == 'GENERIC' or constraintType == 'GENERIC_SPRING':
				rigidBodyConstraintObject["use_limit_lin_x"] = obj.rigid_body_constraint.use_limit_lin_x
				rigidBodyConstraintObject["limit_lin_x_lower"] = obj.rigid_body_constraint.limit_lin_x_lower
				rigidBodyConstraintObject["limit_lin_x_upper"] = obj.rigid_body_constraint.limit_lin_x_upper
				rigidBodyConstraintObject["use_limit_lin_y"] = obj.rigid_body_constraint.use_limit_lin_y
				rigidBodyConstraintObject["limit_lin_y_lower"] = obj.rigid_body_constraint.limit_lin_y_lower
				rigidBodyConstraintObject["limit_lin_y_upper"] = obj.rigid_body_constraint.limit_lin_y_upper
				rigidBodyConstraintObject["use_limit_lin_z"] = obj.rigid_body_constraint.use_limit_lin_z
				rigidBodyConstraintObject["limit_lin_z_lower"] = obj.rigid_body_constraint.limit_lin_z_lower
				rigidBodyConstraintObject["limit_lin_z_upper"] = obj.rigid_body_constraint.limit_lin_z_upper
				rigidBodyConstraintObject["use_limit_ang_x"] = obj.rigid_body_constraint.use_limit_ang_x
				rigidBodyConstraintObject["limit_ang_x_lower"] = obj.rigid_body_constraint.limit_ang_x_lower
				rigidBodyConstraintObject["limit_ang_x_upper"] = obj.rigid_body_constraint.limit_ang_x_upper
				rigidBodyConstraintObject["use_limit_ang_y"] = obj.rigid_body_constraint.use_limit_ang_y
				rigidBodyConstraintObject["limit_ang_y_lower"] = obj.rigid_body_constraint.limit_ang_y_lower
				rigidBodyConstraintObject["limit_ang_y_upper"] = obj.rigid_body_constraint.limit_ang_y_upper
				rigidBodyConstraintObject["use_limit_ang_z"] = obj.rigid_body_constraint.use_limit_ang_z
				rigidBodyConstraintObject["limit_ang_z_lower"] = obj.rigid_body_constraint.limit_ang_z_lower
				rigidBodyConstraintObject["limit_ang_z_upper"] = obj.rigid_body_constraint.limit_ang_z_upper
				if constraintType == 'GENERIC_SPRING':
					rigidBodyConstraintObject["use_spring_x"] = obj.rigid_body_constraint.use_spring_x
					rigidBodyConstraintObject["spring_stiffness_x"] = obj.rigid_body_constraint.spring_stiffness_x
					rigidBodyConstraintObject["spring_damping_x"] = obj.rigid_body_constraint.spring_damping_x
					rigidBodyConstraintObject["use_spring_y"] = obj.rigid_body_constraint.use_spring_y
					rigidBodyConstraintObject["spring_stiffness_y"] = obj.rigid_body_constraint.spring_stiffness_y
					rigidBodyConstraintObject["spring_damping_y"] = obj.rigid_body_constraint.spring_damping_y
					rigidBodyConstraintObject["use_spring_z"] = obj.rigid_body_constraint.use_spring_z
					rigidBodyConstraintObject["spring_stiffness_z"] = obj.rigid_body_constraint.spring_stiffness_z
					rigidBodyConstraintObject["spring_damping_z"] = obj.rigid_body_constraint.spring_damping_z
					rigidBodyConstraintObject["use_spring_ang_x"] = obj.rigid_body_constraint.use_spring_ang_x
					rigidBodyConstraintObject["spring_stiffness_ang_x"] = obj.rigid_body_constraint.spring_stiffness_ang_x
					rigidBodyConstraintObject["spring_damping_ang_x"] = obj.rigid_body_constraint.spring_damping_ang_x
					rigidBodyConstraintObject["use_spring_ang_y"] = obj.rigid_body_constraint.use_spring_ang_y
					rigidBodyConstraintObject["spring_stiffness_ang_y"] = obj.rigid_body_constraint.spring_stiffness_ang_y
					rigidBodyConstraintObject["spring_damping_ang_y"] = obj.rigid_body_constraint.spring_damping_ang_y
					rigidBodyConstraintObject["use_spring_ang_z"] = obj.rigid_body_constraint.use_spring_ang_z
					rigidBodyConstraintObject["spring_stiffness_ang_z"] = obj.rigid_body_constraint.spring_stiffness_ang_z
					rigidBodyConstraintObject["spring_damping_ang_z"] = obj.rigid_body_constraint.spring_damping_ang_z

			jsonObject["constraints"].append(rigidBodyConstraintObject)

	tagObjects = [obj for obj in scene.objects if obj.type == 'EMPTY' and obj.empty_display_type == 'ARROWS']

	buffer.write("tagCount:\t" + str(len(tagObjects)) + "\n")

	for obj in tagObjects:
		print("DUMPING INFO FOR ", obj.name) 
		if obj.type == 'EMPTY' and obj.empty_display_type == 'ARROWS':
			attachmentObject = {}
			attachmentObject["name"] = obj.name
			attachmentObject["position"] = writeVec3Flipped(obj.location)
			attachmentObject["orientation"] = writeQuaternion(obj.rotation_quaternion)

			jsonObject["tags"].append(attachmentObject)

			buffer.write("tag:")
			buffer.write("\tname: " + obj.name)
			buffer.write("\tpos: " + str(obj.location[0]) + " " + str(obj.location[2]) + " " + str(-obj.location[1]))
			buffer.write("\tdim: " + str(obj.dimensions[0]) + " " + str(obj.dimensions[2]) + " " + str(-obj.dimensions[1]))
			writeTxtQuaternion(obj.rotation_quaternion, buffer)
			buffer.write("\n")


	jsonText = json.dumps(jsonObject, indent=4)
	f = open(path, 'w')
	f.write(jsonText)
	f.close()

	textContent = buffer.getvalue()
	txt_path = os.path.splitext(path)[0] + '.txt'
	with open(txt_path, "w") as f:
		f.write(textContent)
	buffer.close()
	
	return {'FINISHED'}

from bpy_extras.io_utils import ExportHelper
from bpy.props import StringProperty, BoolProperty, EnumProperty
from bpy.types import Operator

class Bullet(Operator, ExportHelper):
	bl_idname = "bullet.scene_text"
	bl_label = "Export Bullet data"

	filename_ext = ".bullet"

	filter_glob: StringProperty(
		default=".bullet",
		options={'HIDDEN'},
		maxlen=255
	)

	out_hulls: BoolProperty(
		name="Output Hulls",
		default=True,
	)

	out_meshes: BoolProperty(
		name="Output Meshes",
		default=True
	)

	def execute(self, context):
		return save(context, self.filepath, True, True)



def menu_func_export(self, context):
	self.layout.operator(Bullet.bl_idname, text="Bullet")

def register():
	bpy.utils.register_class(Bullet)
	bpy.types.TOPBAR_MT_file_export.append(menu_func_export)


def unregister():
	bpy.utils.unregister_class(Bullet)
	bpy.types.TOPBAR_MT_file_export.remove(menu_func_export)

if __name__ == "__main__":
	register()
