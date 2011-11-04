//=============================================================================
// Copyright © 2009 NaturalPoint, Inc. All Rights Reserved.
// 
// This software is provided by the copyright holders and contributors "as is" and
// any express or implied warranties, including, but not limited to, the implied
// warranties of merchantability and fitness for a particular purpose are disclaimed.
// In no event shall NaturalPoint, Inc. or contributors be liable for any direct,
// indirect, incidental, special, exemplary, or consequential damages
// (including, but not limited to, procurement of substitute goods or services;
// loss of use, data, or profits; or business interruption) however caused
// and on any theory of liability, whether in contract, strict liability,
// or tort (including negligence or otherwise) arising in any way out of
// the use of this software, even if advised of the possibility of such damage.
//=============================================================================

using System;
using System.Collections.Generic;
using System.Collections;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.Net;

using NatNetML;

/*
 *
 * Simple C# .NET sample showing how to use the NatNet managed assembly (NatNETML.dll).
 * 
 * It is designed to illustrate using NatNet.  There are some inefficiences to keep the
 * code as simple to read as possible.
 * 
 * Sections marked with a [NatNet] are NatNet related and should be implemented in your code.
 * 
 * This sample uses the Microsoft Chart Controls for Microsoft .NET for graphing, which
 * requires the following assemblies:
 *   - System.Windows.Forms.DataVisualization.Design.dll
 *   - System.Windows.Forms.DataVisualization.dll
 * Make sure you have these in your path when building and redistributing.
 * 
 */

namespace WinFormTestApp
{
    public partial class Form1 : Form
    {
        // [NatNet] Our NatNet object
        private NatNetML.NatNetClientML m_NatNet;
        // [NatNet] Our NatNet Frame of Data object
        private NatNetML.FrameOfMocapData m_FrameOfData = new NatNetML.FrameOfMocapData();
    
        NatNetML.ServerDescription desc = new NatNetML.ServerDescription();

        Hashtable htMarkers = new Hashtable();
        Hashtable htRigidBodies = new Hashtable();
        const int GraphFrames = 500;

        int m_iLastFrameNumber = 0;
        float m_fLastFrameTimestamp = 0.0f;
        float m_ServerToMillimeters = 1.0f;
        double m_ServerFramerate = 1.0f;
        float m_fCurrentMocapFrameTimestamp = 0.0f;
		float m_fFirstMocapFrameTimestamp = 0.0f;

        private delegate void OutputMessageCallback(string strMessage);
        private Queue<NatNetML.FrameOfMocapData> m_FrameQueue = new Queue<NatNetML.FrameOfMocapData>();

        public Form1()
        {
            InitializeComponent();
        }

        private void Form1_Load(object sender, EventArgs e)
        {
            // Get the first detected ip address of this machine and use it as a default
            String strMachineName = Dns.GetHostName();
            IPHostEntry ipHost = Dns.GetHostByName(strMachineName);
            int count = 0;
            foreach(IPAddress ip in ipHost.AddressList)
            {
                string strIP = ip.ToString();
                if(count==0)
                {
                    textBoxLocal.Text = strIP;
                    textBoxServer.Text = strIP;
                }
                count++;
            }

            // create NatNet server
            int iConnectionType = 0;
            if (RadioUnicast.Checked)
                iConnectionType = 1;
            int iResult = CreateClient(iConnectionType);

            // create data chart
            chart1.Series[0].Points.Clear();
            chart1.ChartAreas[0].CursorX.IsUserSelectionEnabled = true;

        }

        private int CreateClient(int iConnectionType)
        {
            if(m_NatNet != null)
            {
                m_NatNet.Uninitialize();
            }
            m_NatNet = new NatNetML.NatNetClientML(iConnectionType);

            // [NatNet] set a "Frame Ready" callback function (event handler) handler that will be
            // called by NatNet when NatNet receives a frame of data from the server application
            m_NatNet.OnFrameReady += new NatNetML.FrameReadyEventHandler(m_NatNet_OnFrameReady);

            // [NatNet] print version info
            int[] ver = new int[4];
            ver = m_NatNet.NatNetVersion();
            String strVersion = String.Format("NatNet Version : {0}.{1}.{2}.{3}", ver[0], ver[1],ver[2],ver[3]);
            OutputMessage(strVersion);

            return 0;
            
        }

        private void Connect()
        {
            // [NatNet] connect to a NatNet server
            int returnCode = 0;
            returnCode = m_NatNet.Initialize(textBoxLocal.Text, textBoxServer.Text);
            if (returnCode == 0)
                OutputMessage("Initialization Succeeded.");
            else
            {
                OutputMessage("Error Initializing.");
                checkBoxConnect.Checked = false;
            }

            // [NatNet] validate the connection
            returnCode = m_NatNet.GetServerDescription(desc);
            if (returnCode == 0)
            {
                OutputMessage("Connection Succeeded.");
                OutputMessage("   Server App Name: " + desc.HostApp);
                OutputMessage(String.Format("   Server App Version: {0}.{1}.{2}.{3}", desc.HostAppVersion[0], desc.HostAppVersion[1], desc.HostAppVersion[2], desc.HostAppVersion[3]));
                OutputMessage(String.Format("   Server NatNet Version: {0}.{1}.{2}.{3}", desc.NatNetVersion[0], desc.NatNetVersion[1], desc.NatNetVersion[2], desc.NatNetVersion[3]));
                checkBoxConnect.Text = "Disconnect";

                if (desc.HostApp.Contains("TrackingTools"))
                    m_ServerToMillimeters = 1000.0f;

                // [NatNet] [optional] send a test/response message
                OutputMessage("Sending TestRequest");
                int nBytes = 0;
                byte[] response = new byte[10000];
                int rc = m_NatNet.SendMessageAndWait("TestRequest", out response, out nBytes);
                if (rc == 0)
                {
                    string str = "   Server: " + System.Text.Encoding.ASCII.GetString(response, 0, nBytes);
                    OutputMessage(str);
                }
                else
                {
                    OutputMessage("   Server: No Response.");
                }

                rc = m_NatNet.SendMessageAndWait("FrameRate", out response, out nBytes);
                if (rc == 0)
                {
                    try
                    {
                        m_ServerFramerate = BitConverter.ToSingle(response, 0);
                        OutputMessage(String.Format("   Server Framerate: {0}", m_ServerFramerate));
                    }
                    catch (System.Exception ex)
                    {
                        OutputMessage(ex.Message);
                    }
                }

                m_fCurrentMocapFrameTimestamp = 0.0f;
                m_fFirstMocapFrameTimestamp = 0.0f;

               
            }
            else
            {
                OutputMessage("Error Connecting.");
                checkBoxConnect.Checked = false;
                checkBoxConnect.Text = "Connect";
            }

        }

        private void Disconnect()
        {
            // [NatNet] disconnect
            m_NatNet.Uninitialize();
            checkBoxConnect.Text = "Connect";
        }

        private void checkBoxConnect_CheckedChanged(object sender, EventArgs e)
        {
            if (checkBoxConnect.Checked)
            {
                Connect();
            }
            else
            {
                Disconnect();
            }
        }

        private void OutputMessage(string strMessage)
        {
            if (this.listView1.InvokeRequired)
            {
                // It's on a different thread, so use Invoke.
                OutputMessageCallback d = new OutputMessageCallback(OutputMessage);
                this.Invoke(d, new object[] { strMessage});
            }
            else
            {
                // It's on the same thread, no need for Invoke
                DateTime d = DateTime.Now;
                String strTime = String.Format("{0}:{1}:{2}:{3}", d.Hour, d.Minute, d.Second, d.Millisecond);
                ListViewItem item = new ListViewItem(strTime, 0);
                item.SubItems.Add(strMessage);
                listView1.Items.Add(item);
            }

        }

        // Redraw the chart with data from the selected cell
        private void UpdateChart(int iFrame)
        {
            int f1 = iFrame;

            // Lets only show 500 frames at a time
            iFrame %= GraphFrames;

            // clear graph if we've wrapped, allow for fudge
            if( (m_iLastFrameNumber - iFrame) > 400)
            {
                chart1.Series[0].Points.Clear();
            }

            if (dataGridView1.SelectedCells.Count>0)
            {
                // use only the first selected cell
                DataGridViewCell cell = dataGridView1.SelectedCells[0];
                if (cell.Value == null)
                    return;
                double dValue = 0.0f;
                if (!Double.TryParse(cell.Value.ToString(), out dValue))
                    return;
                chart1.Series[0].Points.AddXY(iFrame, (float)dValue);
            }

            // update red 'cursor' line
            chart1.ChartAreas[0].CursorX.SetCursorPosition(iFrame);

            m_iLastFrameNumber = iFrame;
            m_fLastFrameTimestamp = m_FrameOfData.fLatency;        
        }


        private void UpdateDataGrid()
        {
            // update MarkerSet data
            for (int i = 0; i < m_FrameOfData.nMarkerSets; i++)
            {
                NatNetML.MarkerSetData ms = m_FrameOfData.MarkerSets[i];
                for (int j = 0; j < ms.nMarkers; j++)
                {
                    string strUniqueName = ms.MarkerSetName + j.ToString();
                    int key = strUniqueName.GetHashCode();
                    if(htMarkers.Contains(key))
                    {
                        int rowIndex = (int)htMarkers[key];
                        if (rowIndex >= 0)
                        {
                            dataGridView1.Rows[rowIndex].Cells[1].Value = ms.Markers[j].x;
                            dataGridView1.Rows[rowIndex].Cells[2].Value = ms.Markers[j].y;
                            dataGridView1.Rows[rowIndex].Cells[3].Value = ms.Markers[j].z;
                        }
                    }
                }
            }

            // update RigidBody data
            for (int i = 0; i < m_FrameOfData.nRigidBodies; i++)
            {
                NatNetML.RigidBodyData rb = m_FrameOfData.RigidBodies[i];

                // testing - tools inverts qz for packetizing/sending, so invert it back
                float qx, qy, qw, qz;
                qx = rb.qx;
                qy = rb.qy;// *-1.0f;
                qz = -rb.qz;
                qw = rb.qw;

                // quats coming from Tools are already normalized
                //QuaternionNormalise(ref qx, ref qy, ref qw, ref qz);

                //qy = qy * 0.5f;
                

                int key = rb.ID.GetHashCode();
                if(htRigidBodies.ContainsKey(key))
                {
                    int rowIndex = (int)htRigidBodies[key];
                    if(rowIndex >= 0)
                    {
                        dataGridView1.Rows[rowIndex].Cells[1].Value = rb.x * m_ServerToMillimeters;
                        dataGridView1.Rows[rowIndex].Cells[2].Value = rb.y * m_ServerToMillimeters;
                        dataGridView1.Rows[rowIndex].Cells[3].Value = rb.z * m_ServerToMillimeters;
                        double y, z, x;
                        QuaternionToEuler(qx, qy, qz, qw, out y, out z, out x);
                        //y *= -1.0f;
                        z *= -1.0f;
                        y = RadiansToDegrees(y);
                        z = RadiansToDegrees(z);
                        x = RadiansToDegrees(x);
                        dataGridView1.Rows[rowIndex].Cells[4].Value = y;
                        dataGridView1.Rows[rowIndex].Cells[5].Value = z;
                        dataGridView1.Rows[rowIndex].Cells[6].Value = x;
                              
                        // Marker data associated with this rigid body
                        for (int j = 0; j < rb.nMarkers; j++)
                        {

                        }
                    }
                }   
            }

            // update Skeleton data
            for (int i = 0; i < m_FrameOfData.nSkeletons; i++)
            {
                NatNetML.SkeletonData sk = m_FrameOfData.Skeletons[i];
                for(int j=0; j<sk.nRigidBodies; j++)
                {
                    NatNetML.RigidBodyData rb = sk.RigidBodies[j];
                    int key = rb.ID.GetHashCode();
                    if (htRigidBodies.ContainsKey(key))
                    {
                        int rowIndex = (int)htRigidBodies[key];
                        if (rowIndex >= 0)
                        {
                            dataGridView1.Rows[rowIndex].Cells[1].Value = rb.x;
                            dataGridView1.Rows[rowIndex].Cells[2].Value = rb.y;
                            dataGridView1.Rows[rowIndex].Cells[3].Value = rb.z;
                            double y, z, x;
                            QuaternionToEuler(rb.qx, rb.qy, rb.qz, rb.qw, out y, out z, out x);
                            y = RadiansToDegrees(y);
                            z = RadiansToDegrees(z);
                            x = RadiansToDegrees(x);
                            dataGridView1.Rows[rowIndex].Cells[4].Value = y;
                            dataGridView1.Rows[rowIndex].Cells[5].Value = z;
                            dataGridView1.Rows[rowIndex].Cells[6].Value = x;

                            // Marker data associated with this rigid body
                            for (int k = 0; k < rb.nMarkers; k++)
                            {

                            }
                        }
                    }
                }
            }   // end skeleton update

        }

        // [NatNet] request data descriptions from server app.  
        private void buttonGetDataDescriptions_Click(object sender, EventArgs e)
        {
            dataGridView1.Rows.Clear();
            htMarkers.Clear();
            htRigidBodies.Clear();

            OutputMessage("Retrieving Data Descriptions....");
            List<NatNetML.DataDescriptor> descs = new List<NatNetML.DataDescriptor>();
            bool bSuccess = m_NatNet.GetDataDescriptions(out descs);
            if(bSuccess)
            {
                OutputMessage(String.Format("Retrieved {0} Data Descriptions....", descs.Count));
                foreach (NatNetML.DataDescriptor d in descs)
                {
                    // MarkerSets
                    if (d.type == (int)NatNetML.DataDescriptorType.eMarkerSetData)                    
                    {
                        NatNetML.MarkerSet ms = (NatNetML.MarkerSet)d;                       
                        OutputMessage("MarkerSet");
                        OutputMessage(" Name : " + ms.Name);
                        OutputMessage(String.Format(" Markers ({0}) ",ms.nMarkers));
                        dataGridView1.Rows.Add("MarkerSet: " + ms.Name);
                        for(int i=0; i<ms.nMarkers; i++)
                        {
                            OutputMessage(("  " + ms.MarkerNames[i]));
                            int rowIndex = dataGridView1.Rows.Add("  " + ms.MarkerNames[i]);
                            // MarkerNameIndexToRow map
                            String strUniqueName = ms.Name + i.ToString();
                            int key = strUniqueName.GetHashCode();
                            htMarkers.Add(key, rowIndex); 
                        }
                    }
                    // RigidBodies
                    else if (d.type == (int)NatNetML.DataDescriptorType.eRigidbodyData)             
                    {
                        NatNetML.RigidBody rb = (NatNetML.RigidBody)d;

                        OutputMessage("RigidBody");
                        OutputMessage(" Name : " + rb.Name);
                        OutputMessage(" ID : " + rb.ID);
                        OutputMessage(" ParentID : " + rb.parentID);
                        OutputMessage(" OffsetX : " + rb.offsetx);
                        OutputMessage(" OffsetY : " + rb.offsety);
                        OutputMessage(" OffsetZ : " + rb.offsetz);
                                               
                        int rowIndex = dataGridView1.Rows.Add("RigidBody: "+rb.Name);
                        // RigidBodyIDToRow map
                        int key = rb.ID.GetHashCode();
                        htRigidBodies.Add(key, rowIndex); 
                    }
                    // Skeletons
                    else if (d.type == (int)NatNetML.DataDescriptorType.eSkeletonData)
                    {
                        NatNetML.Skeleton sk = (NatNetML.Skeleton)d;

                        OutputMessage("Skeleton");
                        OutputMessage(" Name : " + sk.Name);
                        OutputMessage(" ID : " + sk.ID);
                        dataGridView1.Rows.Add("Skeleton: " + sk.Name);
                        for (int i = 0; i < sk.nRigidBodies; i++)
                        {
                            RigidBody rb = sk.RigidBodies[i];
                            OutputMessage(" RB Name  : " + rb.Name);
                            OutputMessage(" RB ID    : " + rb.ID);
                            OutputMessage(" ParentID : " + rb.parentID);
                            OutputMessage(" OffsetX  : " + rb.offsetx);
                            OutputMessage(" OffsetY  : " + rb.offsety);
                            OutputMessage(" OffsetZ  : " + rb.offsetz);
                            int rowIndex = dataGridView1.Rows.Add("Bone: " + rb.Name);
                            // RigidBodyIDToRow map
                            int key = rb.ID.GetHashCode();
                            if (htRigidBodies.ContainsKey(key))
                                MessageBox.Show("Duplicate RigidBody ID");
                            else
                                htRigidBodies.Add(key, rowIndex);
                        }
                    }
                    else
                    {
                        OutputMessage("Unknown DataType");
                    }
                }
            }
            else
            {
                OutputMessage("Unable to retrieve DataDescriptions");
            }
        }

        // [NatNet] m_NatNet_OnFrameReady will be called when a frame of Mocap data has is received
        // from the server application.
        void m_NatNet_OnFrameReady(NatNetML.FrameOfMocapData data, NatNetML.NatNetClientML client)
        {
            m_FrameQueue.Enqueue(data);           
        }

        double RadiansToDegrees(double dRads)
        {
            return dRads * (180.0f / Math.PI);
        }

        // normalising a quaternion works similar to a vector. This method will not do anything
        // if the quaternion is close enough to being unit-length. define TOLERANCE as something
        // small like 0.00001f to get accurate results
        public void QuaternionNormalise(ref float x, ref float y, ref float z, ref float w)
        {
	        // Don't normalize if we don't have to
            float tolerance = 0.00001f;
            float mag2 = w * w + x * x + y * y + z * z;
            if (mag2 != 0.0f && (Math.Abs(mag2 - 1.0f) > tolerance))
            {
                float mag = (float)Math.Sqrt(mag2);
                w /= mag;
                x /= mag;
                y /= mag;
                z /= mag;
            }
	    }

        // Convert a quaternion (TrackingTools type) to euler angles (Y, Z, X)
        // Y = Heading  (Yaw)
        // Z = Attitude (Pitch)
        // X = Bank     (Roll)
        // From Martin Baker (http://www.euclideanspace.com/maths/geometry/rotations/conversions/quaternionToEuler/index.htm)
        // conventions:
        //  - input and output units are both in radians
        //  - euler order is YZX
        //  - euler angles are about global axes
        //  - euler + angle is right-handed
        private void QuaternionToEuler(double qx, double qy, double qz, double qw, out double y, out double z, out double x)
        {
            double test = qx * qy + qz * qw;
            if (test > 0.499)                            // singularity at north pole
            {              
                y = 2.0F * System.Math.Atan2(qx, qw);
                z = Math.PI / 2.0F;
                x = 0.0F;
                return;
            }

            if (test < -0.499)                           // singularity at south pole
            {
                y = -2.0F * System.Math.Atan2(qx, qw);
                z = -Math.PI / 2.0F;
                x = 0.0F;
                return;
            }

            double sqx = qx * qx;
            double sqy = qy * qy;
            double sqz = qz * qz;
            y = System.Math.Atan2(2.0F * qy * qw - 2.0 * qx * qz, 1.0F - 2.0 * sqy - 2.0 * sqz);
            z = System.Math.Asin(2.0F * test);
            x = System.Math.Atan2(2.0 * qx * qw - 2.0 * qy * qz, 1.0F - 2.0 * sqx - 2.0 * sqz);
        }

        private void Form1_FormClosing(object sender, FormClosingEventArgs e)
        {
            m_NatNet.Uninitialize();
        }

        // UI refresher
        private void UpdateUITimer_Tick(object sender, EventArgs e)
        {
            lock (m_FrameQueue)
            {
                while (m_FrameQueue.Count > 0)
                {
                    m_FrameOfData = m_FrameQueue.Dequeue();

                    // for servers that only use timestamps, not frame numbers, calculate a 
                    // frame number from the time delta between frames
                    if (desc.HostApp.Contains("TrackingTools"))
                    {                      
                        m_fCurrentMocapFrameTimestamp = m_FrameOfData.fLatency;
                        if (m_fFirstMocapFrameTimestamp == 0.0f)
                        {
                            m_fFirstMocapFrameTimestamp = m_fCurrentMocapFrameTimestamp;
                        }
                        m_FrameOfData.iFrame = (int)((m_fCurrentMocapFrameTimestamp - m_fFirstMocapFrameTimestamp) * m_ServerFramerate);
                    }

                    // update the data grid
                    UpdateDataGrid();

                    // update the chart
                    UpdateChart(m_FrameOfData.iFrame);
                    // only redraw chart when necessary, not for every frame
                    if (m_FrameQueue.Count == 0)
                    {
                        chart1.ChartAreas[0].RecalculateAxesScale();
                        chart1.ChartAreas[0].AxisX.Minimum = 0;
                        chart1.ChartAreas[0].AxisX.Maximum = GraphFrames;
                        chart1.Invalidate();
                    }
                }
            }
        }

        private void RadioMulticast_CheckedChanged(object sender, EventArgs e)
        {
            bool bNeedReconnect = checkBoxConnect.Checked;
            int iResult = CreateClient(0);
            if (bNeedReconnect)
                Connect();               
        }

        private void RadioUnicast_CheckedChanged(object sender, EventArgs e)
        {
            bool bNeedReconnect = checkBoxConnect.Checked;
            int iResult = CreateClient(1);
            if (bNeedReconnect)
                Connect();                         
        }

    }
}