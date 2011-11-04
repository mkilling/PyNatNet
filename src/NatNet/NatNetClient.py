import cnatnet

class NatNetClient:
	def __init__(self, iType):
		self.inst = cnatnet.constructor(iType)
	
	def NatNetVersion(self):
		return cnatnet.natNetVersion(self)
	
	def SetMessageCallback(self, messageHandler):
		pass
	
	def SetVerbosityLevel(self, verbosityLevel):
		pass
	
	def SetDataCallback(self, dataCallback):
		pass
	
	def Initialize(self, myIpAddress, serverIPAddress):
		pass
	
	def GetServerDescription(self):
		pass
