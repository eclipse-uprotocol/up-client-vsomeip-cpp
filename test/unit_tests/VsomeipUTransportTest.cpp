#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "VsomeipUTransport.hpp"
#include "vsomeip/vsomeip.hpp"
#include "mock/UURIHelper.hpp"
#include <VsomeipUTransport.hpp>
#include <up-cpp/transport/builder/UAttributesBuilder.h>
#include <up-core-api/uri.pb.h>
#include <up-cpp/uuid/factory/Uuidv8Factory.h>
#include <up-cpp/uuid/serializer/UuidSerializer.h>
#include <up-client-vsomeip-cpp/transport/VsomeipUTransport.hpp>
#include "mock/CustomEventListener.hpp"

using namespace uprotocol::utransport;
using namespace uprotocol::uuid;
using namespace uprotocol::v1;

/**
 *  @brief Payload data.
 */
static uint8_t const g_data[4] = "100";

/**
 *  @brief Translate a UMessage to a string.
 *
 *  @param type - the UMessage to be translated.
 *
 *  @return string - The UMessage type as a string.
 */
std::string UMessageTypeToString(UMessageType type) {
    switch (type) {
        case UMessageType::UMESSAGE_TYPE_PUBLISH:
            return "UMESSAGE_TYPE_PUBLISH";
        case UMessageType::UMESSAGE_TYPE_REQUEST:
            return "UMESSAGE_TYPE_REQUEST";
        case UMessageType::UMESSAGE_TYPE_RESPONSE:
            return "UMESSAGE_TYPE_RESPONSE";
        default:
            return "Unknown UMessageType";
    }
}

/**
 *  @brief Translate a UPriority to a string.
 *
 *  @param priority - The UPriority to be translated.
 *
 *  @return string - the UPriority type as a string.
 */
std::string UPriorityToString(UPriority priority) {
    switch (priority) {
        case UPriority::UPRIORITY_CS0:
            return "UPRIORITY_CS0";
        case UPriority::UPRIORITY_CS1:
            return "UPRIORITY_CS1";
        case UPriority::UPRIORITY_CS2:
            return "UPRIORITY_CS2";
        case UPriority::UPRIORITY_CS3:
            return "UPRIORITY_CS3";
        case UPriority::UPRIORITY_CS4:
            return "UPRIORITY_CS4";
        case UPriority::UPRIORITY_CS5:
            return "UPRIORITY_CS5";
        case UPriority::UPRIORITY_CS6:
            return "UPRIORITY_CS6";
        default:
            return "Unknown UPriority";
    }
}

/**
 *  @brief Implements the abstract class UListener which is needed for testing VsomeipUTransport.
 */
class TestListener : public UListener {
public:
    UStatus onReceive(UMessage &message) const {
        (void)message;
        UStatus status;
        status.set_code(UCode::OK);
        return status;
    }

};

/**
 *  @brief Test fixture for VsomeipUTransport.
 */
class VsomeipUTransportTests : public testing::Test {
protected:
    /**
     *  @brief payload for message.
     */
    UPayload payloadForTransport = UPayload(g_data, sizeof(g_data), UPayloadType::VALUE);

    /**
     *  @brief Assets needed to make a message.
     */
    UUID uuidForTransport = Uuidv8Factory::create();
    std::shared_ptr<UUri> testUURI = buildUURI();
    UPriority const priority = UPriority::UPRIORITY_CS4;
    UMessageType const publishType = UMessageType::UMESSAGE_TYPE_PUBLISH;
    UAttributesBuilder builderForTransport = UAttributesBuilder(*testUURI, uuidForTransport, publishType, priority);
    UAttributes attributesForTransport = builderForTransport.build();

    /**
     *  @brief UListener object for testing.
     */
    TestListener listener_;

    /**
     *  @brief Setup for VsomeipUTransport.
     */
    void SetUp() override {
    }
    
    /**
     *  @brief Teardown for VsomeipUTransport
     */
    void TearDown() override {
    }
};

/**
 *  @brief Test that unregisterListener properly unregisters a UListener. Not yet implemented.
 */
TEST_F(VsomeipUTransportTests, unRegisterListenerTest) {
    TestListener listener_;
    UStatus status = VsomeipUTransport::instance().unregisterListener(*this->testUURI, listener_);

    EXPECT_EQ(status.code(), UCode::UNIMPLEMENTED);
}

/**
 *  @brief Test that a payload is received when receive is called. not yet implemented.
 */
TEST_F(VsomeipUTransportTests, receiveTest) {
    UStatus status = VsomeipUTransport::instance().receive(*this->testUURI,
                                                           this->payloadForTransport,
                                                           attributesForTransport);

    EXPECT_EQ(status.code(), UCode::UNIMPLEMENTED);
}

/**
 *  @brief Test that terminate function properly terminates the VsomeipUTransport.
 */
TEST_F(VsomeipUTransportTests, terminateTest) {
    UStatus status = VsomeipUTransport::instance().terminate();

    EXPECT_EQ(status.code(), UCode::OK);
}
