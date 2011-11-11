import cnatnet

class NatNetClient:
    def __init__(self, iType):
        self.inst = cnatnet.constructor(iType)

    def NatNetVersion(self):
        return cnatnet.natNetVersion(self.inst)

    def SetMessageCallback(self, messageHandler):
        cnatnet.setMessageCallback(self.inst, messageHandler)

    def SetVerbosityLevel(self, verbosityLevel):
        cnatnet.setVerbosityLevel(self.inst, verbosityLevel)

    def SetDataCallback(self, dataCallback):
        cnatnet.setDataCallback(self.inst, dataCallback)

    def Initialize(self, myIpAddress, serverIpAddress):
        print cnatnet.initialize(self.inst, myIpAddress, serverIpAddress)

    def GetDataDescriptions(self):
        return cnatnet.getDataDescription(self.inst)

    def GetServerDescription(self):
        return cnatnet.getServerDescription(self.inst)
