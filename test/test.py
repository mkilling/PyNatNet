import NatNet

c = NatNet.NatNetClient(1)
print c.NatNetVersion()

c.SetVerbosityLevel(NatNet.Verbosity_Info)

c.Initialize("", "")


def onData(dataFrame):
    print dataFrame

c.SetDataCallback(onData)

while True:
    pass

