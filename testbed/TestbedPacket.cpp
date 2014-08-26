/*
 * $FU-Copyright$
 */

#include "TestbedPacket.h"
#include "TestbedPacketFactory.h"

TESTBED_NAMESPACE_BEGIN

TestbedPacket::TestbedPacket(AddressPtr source, AddressPtr destination, AddressPtr sender, char type, unsigned int seqNr, int ttl, const char* payload, unsigned int payloadSize) : Packet(source, destination, sender, type, seqNr, ttl, payload, payloadSize) { }

AddressPtr TestbedPacket::getSource() const {
    std::lock_guard<std::mutex> lock(sourceMutex);
    return Packet::getSource();
}

AddressPtr TestbedPacket::getDestination() const {
    std::lock_guard<std::mutex> lock(destinationMutex);
    return Packet::getDestination();
}

AddressPtr TestbedPacket::getSender() const {
    std::lock_guard<std::mutex> lock(senderMutex);
    return Packet::getSender();
}

AddressPtr TestbedPacket::getPreviousHop() const {
    std::lock_guard<std::mutex> lock(mutex);
    return Packet::getPreviousHop();
}

void TestbedPacket::addPayload(dessert_msg_t* message) {
    /// check if the packet has actually payload  
    if (ntohs(message->plen) > 0) {
        void* originalPayload = nullptr;

        // DEBUG: std::cerr << "[TestbedPacket::addPayload] payload is " << message->plen << " and with ntohs(msg->plen) it is " << ntohs(message->plen) << std::endl;

        if (ntohs(dessert_msg_getpayload(message, &originalPayload)) == ntohs(message->plen)){
            std::lock_guard<std::mutex> lock(mutex);

            if (payload != nullptr) {
                delete[] payload;
                payloadSize = 0;
            }
            payloadSize = ntohs(message->plen);

            char* tmpPayload = new char[payloadSize];
            std::memcpy(tmpPayload, (char*)originalPayload, payloadSize);

            payload = tmpPayload;
        } else {
            // DEBUG:
            std::cerr << "[TestbedPacket::setMessage] saving payload failed" << std::endl;
        }
    } else {
        // DEBUG:
        std::cerr << "[TestbedPacket::setMessage] tried to save payload while there is actually none" << std::endl;
    }
}

dessert_msg_t* TestbedPacket::toDessertMessage() const {
    std::lock_guard<std::mutex> lock(mutex);

    ara_address_t address;
    dessert_msg_t* packet = nullptr;
    dessert_ext_t* extension = nullptr;
    struct ether_header* ethernetHeader = nullptr;

    // get a libdessert representation of the destination address
    std::shared_ptr<TestbedAddress> destinationAddress = std::dynamic_pointer_cast<TestbedAddress>(destination);
    std::copy(destinationAddress->getDessertValue(), destinationAddress->getDessertValue() + ETHER_ADDR_LEN, address);

    // get a libdessert representation of the destination address
    std::shared_ptr<TestbedAddress> sourceAddress = std::dynamic_pointer_cast<TestbedAddress>(source);

    /// create a new dessert_msg_t
    if (dessert_msg_new(&packet) == 0) {
        /// set sequence number
        packet->u16 = htons(seqNr);
        /// set time to live 
        packet->ttl = (uint8_t)ttl;
        /// set the type
        //packet->u8 |= type;
        packet->u8 = type;
        /// TODO: ant size in DES-ARA set to 128 by means of des-ara.conf
        int antSize = 128;

        dessert_msg_addext(packet, &extension, DESSERT_EXT_ETH, ETHER_HDR_LEN);
        ethernetHeader = (struct ether_header*) extension->data;
        /// set the previous hop
        memcpy(ethernetHeader->ether_shost, DESSERT_LOCAL_ADDRESS, ETHER_ADDR_LEN);

        /*
        u_int8_t* src = sourceAddress->getDessertValue();
        u_int8_t* dest = destinationAddress->getDessertValue();
        */

        if ((type == PacketType::FANT) || (type == PacketType::BANT)) {
            /// set the destination 
            memcpy(ethernetHeader->ether_dhost, DESSERT_BROADCAST_ADDRESS, ETHER_ADDR_LEN);

            if (type == PacketType::FANT) {
                dessert_msg_addext(packet, &extension, ARA_EXT_FANT, ETHER_ADDR_LEN + 4);
                memcpy(extension->data, address, sizeof(ara_address_t));
                memcpy(extension->data + ETHER_ADDR_LEN, "FANT", 4);
            } else {
                dessert_msg_addext(packet, &extension, ARA_EXT_BANT, ETHER_ADDR_LEN + 4);
                memcpy(extension->data, address, sizeof(ara_address_t));
                memcpy(extension->data + ETHER_ADDR_LEN, "BANT", 4);
            }
        /*    
        dessert_ext_t* routingExtension = nullptr;
        dessert_msg_addext(packet, &routingExtension, DESSERT_EXT_USER, ETHER_HDR_LEN);
        RoutingExtension* araRoutingExtension = (RoutingExtension*)routingExtension->data;
        /// TODO: and again better check this
        memcpy(araRoutingExtension->ara_shost, src, ETHER_ADDR_LEN);
        memcpy(araRoutingExtension->ara_dhost, dest, ETHER_ADDR_LEN);
*/
            dessert_msg_dummy_payload(packet, antSize);
        } else if (type == PacketType::DATA){
            void* tempPayload = nullptr;

            /**
             * TODO: this might cause a lot of trouble, depending what is doing
             * the free or (delete). We assume that libdessert takes care of the
             * packet and the payload and hence, it is very likely that free is
             * called.
             */
            if ((tempPayload = malloc(payloadSize)) != nullptr){
                /// copy over the original payload
                memcpy(tempPayload, payload, payloadSize);

                if (dessert_msg_addpayload(packet, &tempPayload, payloadSize) == DESSERT_OK) {

                } else {
                   // DEBUG:
                   std::cerr << "[TestbedPacket::toDessertMessage] adding payload failed" << std::endl;
                }
            } else {
                // DEBUG:
                std::cerr << "[TestbedPacket::toDessertMessage] memory allocation for payload failed" << std::endl;
            }
        } else if (type == PacketType::PANT){

        } else if (type == PacketType::ACK){

        } else if (type == PacketType::DUPLICATE_ERROR){

        } else if (type == PacketType::ROUTE_FAILURE){

        } else if (type == PacketType::HELLO){

        } else if (type == PacketType::PEANT){

        } else {
            throw Exception("Unsupported packet type in makeAntAgent() method");
        }
    /// creating a dessert_msg_t failed
    } else {
        // DEBUG:
        std::cerr << "[TestbedPacket::toDessertMessage] creating a new dessert message failed" << std::endl;
    }

    return packet;
}


TESTBED_NAMESPACE_END