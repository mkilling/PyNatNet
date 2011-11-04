import NatNet
import time

c = NatNet.NatNetClient(1)
print c.NatNetVersion()

c.SetVerbosityLevel(NatNet.Verbosity_Info)

c.Initialize("", "")


def onData(dataFrame):
    body = dataFrame.RigidBodies[0]
    print "x %.2f  y %.2f  z %.2f" % (body.x, body.y, body.z)

c.SetDataCallback(onData)

while True:
	pass

