#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <vsomeip/vsomeip.hpp>
#include <memory>
#include <vector>
#include "SomeipHandler.hpp"
#include "MessageTranslator.hpp"
#include "mock/MockSomeipInterface.hpp"
#include "mock/MockApplication.hpp"
#include <VsomeipUTransport.hpp>
#include <up-cpp/transport/builder/UAttributesBuilder.h>
#include <up-core-api/uri.pb.h>
#include <up-cpp/uuid/factory/Uuidv8Factory.h>
#include "SomeipWrapper.hpp"
#include "UURIHelper.hpp"
#include "mock/MockPayload.hpp"

using ::testing::Return;
using namespace uprotocol::utransport;
using namespace uprotocol::uuid;
using namespace uprotocol::v1;
using ::testing::NiceMock;
using namespace vsomeip_v3;
using ::testing::AtLeast;

/**
 *  @brief payload data.
 */
static uint8_t g_data[4] = "100";

/**
 *  @brief Test fixture for MessageTranslator.
 */
class MessageTranslatorTests : public testing::Test {
protected:
    /**
     *  @brief Assets needed to make a message.
     */
    UUID uuidForTranslator = Uuidv8Factory::create();
    std::shared_ptr<UUri> testUURIForTranslator = buildUURI();
    UPriority const priority = UPriority::UPRIORITY_CS4;
    UMessageType const publishTypeForTranslator = UMessageType::UMESSAGE_TYPE_PUBLISH;
    UAttributesBuilder builderForTranslator = UAttributesBuilder(*testUURIForTranslator,
                                                                    uuidForTranslator,
                                                                    publishTypeForTranslator,
                                                                    priority);
    UAttributes attributesForTranslator = builderForTranslator.build();

    uint16_t const uEntityId             = 0x1102; //Service ID
    std::string const uEntityName        = "0x1102";
    uint32_t const uEntityVersionMajor   = 0x1; //Major Version
    uint32_t const uEntityVersionMinor   = 0x0; //Minor Version
    uint16_t const uResourceId           = 0x0102; //Method ID
    std::string const uResourceName      = "rpc";
    std::string const uResourceInstance  = "0x0102";

    /**
     *  @brief payload for message.
     */
    UPayload payloadForTranslator = UPayload(g_data, sizeof(g_data), UPayloadType::VALUE);

    /**
     *  @brief UMessage used for testing.
     */
    UMessage messageForTranslator = UMessage(payloadForTranslator, attributesForTranslator);

    /**
     *  @brief Wrapper object used as a SomeipInterface object for MessageTranslator.
     */
    inline static SomeipWrapper someIpWrapperInstance_;
    /**
     *  @brief MessageTranslator object for testing.
     */
    MessageTranslator translator;
    MessageTranslatorTests() : translator(someIpWrapperInstance_) {}
    std::shared_ptr<uprotocol::utransport::UMessage> shared_message =
    std::make_shared<uprotocol::utransport::UMessage>(messageForTranslator);

    std::shared_ptr<::testing::NiceMock<MockApplication>> mockApp_;

    /**
     *  @brief Setup for MessageTranslator that is shared between tests.
     */
    static void SetUpTestCase() {
    }

    /**
     *  @brief Teardown for MessageTranslator that is shared between tests.
     */
    static void TearDownTestCase() {
    }

    /**
     *  @brief Setup for MessageTranslator.
     */
    void SetUp() override {
    }

    /**
     *  @brief Teardown for MessageTranslator.
     */
    void TearDown() override {
    }
};

/**
 *  @brief Test to verify that a UMessage is correctly translated to a Someip message for a request.
 */
TEST_F(MessageTranslatorTests, translateUMessageToSomeipMsgForRequestTest) {
    std::shared_ptr<message> message = translator.translateUMessageToSomeipMsgForRequest(shared_message);
    EXPECT_EQ(message->get_method(), this->attributesForTranslator.sink().resource().id());
    EXPECT_EQ(message->get_service(), shared_message->attributes().sink().entity().id());
    EXPECT_EQ(message->get_instance(), 0);
}

/**
 *  @brief Test to verify a Someip message is correctly translated to a UMessage for a request.
 */
TEST_F(MessageTranslatorTests, translateSomeipToUMessageForRequestTest) {
    UEntity uEntity;
    UResource uResource;

    uEntity.set_id(this->uEntityId);
    uEntity.set_name(this->uEntityName.c_str());
    uEntity.set_version_major(this->uEntityVersionMajor);
    uEntity.set_version_minor(this->uEntityVersionMinor);

    uResource.set_id(this->uResourceId);
    uResource.set_name(this->uResourceName.c_str());
    uResource.set_instance(this->uResourceInstance);

    std::shared_ptr<message> message = translator.translateUMessageToSomeipMsgForRequest(shared_message);
    std::shared_ptr<UMessage> result = translator.translateSomeipToUMsgForRequest(message, uEntity, uResource);

    EXPECT_NE(result, nullptr);
    EXPECT_EQ(result->attributes().type(), UMessageType::UMESSAGE_TYPE_REQUEST);
    EXPECT_EQ(result->attributes().priority(), UPriority::UPRIORITY_CS4);
}

/**
 *  @brief Test to verify a Someip message is correctly translated to a UMessage for an acknowledgement.
 */
TEST_F(MessageTranslatorTests, translateSomeipToUMsgForSubscriptionAckTest) {
    UEntity uEntity;
    UResource uResource;
    UAuthority uAuthority;

    uEntity.set_id(this->uEntityId);
    uEntity.set_name(this->uEntityName.c_str());
    uEntity.set_version_major(this->uEntityVersionMajor);
    uEntity.set_version_minor(this->uEntityVersionMinor);

    uResource.set_id(this->uResourceId);
    uResource.set_name(this->uResourceName.c_str());
    uResource.set_instance(this->uResourceInstance);

    std::shared_ptr<UMessage> result = translator.translateSomeipToUMsgForSubscriptionAck(uEntity,
                                                                                          uAuthority,
                                                                                          uResource);

    EXPECT_EQ(result->attributes().type(), UMessageType::UMESSAGE_TYPE_UNSPECIFIED);
    EXPECT_EQ(result->attributes().priority(), UPriority::UPRIORITY_CS0);
}

/**
 *  @brief Test to verify a Someip message is correctly translated to a UMessage for a notification.
 */
TEST_F(MessageTranslatorTests, translateSomeipToUMsgForNotificationTest) {
    std::shared_ptr<message> message = translator.translateUMessageToSomeipMsgForRequest(shared_message);
    UEntity uEntity;
    uEntity.set_id(0x1102);
    uEntity.set_name("0x1102");
    uEntity.set_version_major(1);
    uEntity.set_version_minor(0);

    UAuthority uAuthority;
    uAuthority.set_ip("172.17.0.1");

    UResource uResource;
    uResource.set_id(0x0102);
    uResource.set_name("rpc");
    uResource.set_instance("0x0102");

    std::shared_ptr<UMessage> uMessage = translator.translateSomeipToUMsgForNotification(message,
                                                                                         uEntity,
                                                                                         uAuthority,
                                                                                         uResource);

    EXPECT_EQ(uMessage->attributes().type(), UMessageType::UMESSAGE_TYPE_PUBLISH);
    EXPECT_EQ(uMessage->attributes().priority(), UPriority::UPRIORITY_CS0);
}

/**
 *  @brief Test to verify a Someip message is correctly translated to a UMessage for a response.
 */
TEST_F(MessageTranslatorTests, translateSomeipToUMsgForResponseTest) {
    std::shared_ptr<message> message = translator.translateUMessageToSomeipMsgForRequest(shared_message);
    std::shared_ptr<uprotocol::utransport::UMessage> originalRequestMsg =
        std::make_shared<uprotocol::utransport::UMessage>(messageForTranslator);
    std::shared_ptr<UMessage> uMsg = translator.translateSomeipToUMsgForResponse(message, originalRequestMsg);

    EXPECT_EQ(uMsg->attributes().type(), UMessageType::UMESSAGE_TYPE_RESPONSE);
    EXPECT_EQ(uMsg->attributes().priority(), UPriority::UPRIORITY_CS4);
    EXPECT_EQ(uprotocol::uuid::UuidSerializer::serializeToString(uMsg->attributes().id()),
              uprotocol::uuid::UuidSerializer::serializeToString(this->uuidForTranslator));
}
