import cnatnet

class NatNetClient:
    def __init__(self, iType):
        self.inst = cnatnet.constructor(iType)
        self.data_callback_user_data = None
    
    def NatNetVersion(self):
        return cnatnet.natNetVersion(self.inst)
    
    def SetMessageCallback(self, messageHandler):
        cnatnet.setMessageCallback(self.inst, messageHandler)
    
    def SetVerbosityLevel(self, verbosityLevel):
        cnatnet.setVerbosityLevel(self.inst, verbosityLevel)
    
    def SetDataCallback(self, dataCallback, userData):
        self.data_callback_user_data = userData
        cnatnet.data_callback = dataCallback
        cnatnet.setDataCallback(self.inst, self._onDataCallback)
        
    def _onDataCallback(self, dataFrame):
        self.data_callback(dataFrame, self.data_callback_user_data)
    
    def Initialize(self, myIpAddress, serverIpAddress):
        cnatnet.initialize(self.inst, myIpAddress, serverIpAddress)
    
    def GetServerDescription(self):
        return cnatnet.getServerDescription(self.inst)
