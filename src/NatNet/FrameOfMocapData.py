class FrameOfMocapData:
    def __init__(self, iFrame, rigidBodies, latency):
        self.iFrame = iFrame
        self.RigidBodies = rigidBodies
        self.Latency = latency
