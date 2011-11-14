import cnatnet

class NatNetClient:
    def __init__(self, iType):
        self.inst = cnatnet.constructor(iType)
        self.lastFrame = None
        self.dataCallback = None
        cnatnet.setDataCallback(self.inst, self._callback)

    def Initialize(self, myIpAddress, serverIpAddress):
        print cnatnet.initialize(self.inst, myIpAddress, serverIpAddress)

    def Uninitialize(self):
        return cnatnet.uninitialize(self.inst)

    def NatNetVersion(self):
        return cnatnet.natNetVersion(self.inst)

    def SetMessageCallback(self, messageHandler):
        cnatnet.setMessageCallback(self.inst, messageHandler)

    def SetVerbosityLevel(self, verbosityLevel):
        cnatnet.setVerbosityLevel(self.inst, verbosityLevel)

    def SetDataCallback(self, dataCallback):
        self.dataCallback = dataCallback

    def GetDataDescriptions(self):
        return cnatnet.getDataDescription(self.inst)

    def GetServerDescription(self):
        return cnatnet.getServerDescription(self.inst)

    def GetLastFrameOfData(self):
        return self.lastFrame

    def _callback(self, dataFrame):
        self.lastFrame = dataFrame
        if self.dataCallback:
            self.dataCallback(dataFrame)
