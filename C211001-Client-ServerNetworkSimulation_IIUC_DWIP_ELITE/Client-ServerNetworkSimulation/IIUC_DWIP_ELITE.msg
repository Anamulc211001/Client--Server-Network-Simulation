//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 1992-2015 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//


enum IIUC_DWIP_ELITEType
{
    IIUC_CONN_REQ = 0;
    IIUC_CONN_ACK = 1;
    IIUC_DISC_REQ = 2;
    IIUC_DISC_ACK = 3;
    IIUC_DATA = 4;
};

packet IIUC_DWIP_ELITE
{
    int srcAddress;
    int destAddress;
    int serverProcId;
};

packet IIUC_DWIP_ELITEDataPacket extends IIUC_DWIP_ELITE
{
    string payload;
};

