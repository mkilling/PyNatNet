class MarkerSetDescription:
	def __init__(self, name, markerNames):
		self.Name = name
		self.MarkerNames = markerNames

class RigidBodyDescription:
	def __init__(self, name, id, parentID, offsetx, offsety, offsetz):
		self.Name = name
		self.ID = id
		self.parentID = parentID
		self.offsetx = offsetx
		self.offsety = offsety
		self.offsetz = offsetz

class SkeletonDescription:
	def __init__(self, name, skeletonID, rigidBodies):
		self.Name = name
		self.skeletonID = skeletonID
		self.RigidBodies = rigidBodies
