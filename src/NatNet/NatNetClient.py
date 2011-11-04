import cnatnet

class NatNetClient:
    def __init__(self, iType):
        self.inst = cnatnet.constructor(iType)
    
    def NatNetVersion(self):
        return cnatnet.natNetVersion(self.inst)
    
    def SetMessageCallback(self, messageHandler):
        cnatnet.setMessageCallback(self, messageHandler)
    
    def SetVerbosityLevel(self, verbosityLevel):
        cnatnet.setVerbosityLevel(self.inst, verbosityLevel)
    
    def SetDataCallback(self, dataCallback):
        pass
    
    def Initialize(self, myIpAddress, serverIPAddress):
        pass
    
    def GetServerDescription(self):
        pass
