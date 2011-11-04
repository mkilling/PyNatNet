import NatNet

c = NatNet.NatNetClient(1)
print c.NatNetVersion()

c.SetVerbosityLevel(NatNet.Verbosity_Error)

c.Initialize("127.0.0.1", "127.0.0.1")
