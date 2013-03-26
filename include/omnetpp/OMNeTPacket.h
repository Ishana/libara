/*
 * $FU-Copyright$
 */

#ifndef _OMNETPACKET_H_
#define _OMNETPACKET_H_

#include "OMNeTARAMacros.h"
#include "Packet.h"
#include "Address.h"
#include "OMNeTAddress.h"
#include "IPAddress.h"

OMNETARA_NAMESPACE_BEGIN

class OMNeTPacket : public cPacket, public Packet {
  private:
    void copy(const OMNeTPacket& other);

  protected:
    // protected and unimplemented operator==(), to prevent accidental usage
    bool operator==(const OMNeTPacket&);

  public:
    OMNeTPacket(std::shared_ptr<Address> source=nullptr, std::shared_ptr<Address> destination=nullptr, std::shared_ptr<Address> sender=nullptr, char type=0, unsigned int seqNr=0, unsigned int hopCount = 0, const char* payload=nullptr, unsigned int payloadSize=0);
    OMNeTPacket(const OMNeTPacket& other);
    OMNeTPacket& operator=(const OMNeTPacket& other);
    virtual OMNeTPacket *dup() const {return new OMNeTPacket(*this);}
    virtual void parsimPack(cCommBuffer *b);
    virtual void parsimUnpack(cCommBuffer *b);

    std::shared_ptr<OMNeTAddress> getSource() const;
    std::shared_ptr<OMNeTAddress> getDestination() const;
    std::shared_ptr<OMNeTAddress> getSender() const;

    IPAddress getSourceIP() const { return *(getSource().get()); }
    IPAddress getDestinationIP() const { return *(getDestination().get()); }
    IPAddress getSenderIP() const { return *(getSender().get()); }
};

inline void doPacking(cCommBuffer *b, OMNeTPacket& obj) {obj.parsimPack(b);}
inline void doUnpacking(cCommBuffer *b, OMNeTPacket& obj) {obj.parsimUnpack(b);}

OMNETARA_NAMESPACE_END

#endif // _OMNETPACKET_H_
