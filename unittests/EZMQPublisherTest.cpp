/*******************************************************************************
 * Copyright 2017 Samsung Electronics All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 *******************************************************************************/

#include "UnitTestHelper.h"
#include "EZMQAPI.h"
#include "EZMQLogger.h"
#include "EZMQPublisher.h"

#include <iostream>

#define TAG "EZMQ_PUB_TEST"
static int mPort = 5562;
using namespace ezmq;

void startCB(EZMQErrorCode /*code*/)
{
    EZMQ_LOG(DEBUG, TAG, "Start callback");
}

void stopCB(EZMQErrorCode /*code*/)
{
    EZMQ_LOG(DEBUG, TAG, "stop callback");
}

void errorCB(EZMQErrorCode /*code*/)
{
    EZMQ_LOG(DEBUG, TAG, "error callback");
}

class EZMQPublisherTest: public TestWithMock
{
    protected:
        void SetUp()
        {
            mTopic = "topic";
            mPort = mPort +1;
            apiInstance = EZMQAPI::getInstance();
            ASSERT_NE(nullptr, apiInstance);
            EXPECT_EQ(EZMQ_OK, apiInstance->initialize());
            mPublisher = new EZMQPublisher(mPort, startCB, stopCB, errorCB);
            TestWithMock::SetUp();
        }

        void TearDown()
        {
            mPublisher->stop();
            apiInstance->terminate();
            TestWithMock::TearDown();
        }

        EZMQAPI *apiInstance;
        EZMQPublisher *mPublisher;
        std::string  mTopic;
};

TEST_F(EZMQPublisherTest, constructor)
{
    EZMQPublisher *instance = new EZMQPublisher(mPort, startCB, stopCB, errorCB);
    ASSERT_NE(nullptr, instance);
}

TEST_F(EZMQPublisherTest, startstop)
{
    for( int i =1; i<=10; i++)
    {
        EXPECT_EQ(EZMQ_OK, mPublisher->start());
        EXPECT_EQ(EZMQ_OK, mPublisher->stop());
    }
}

TEST_F(EZMQPublisherTest, start)
{
    EXPECT_EQ(EZMQ_OK, mPublisher->start());
}

TEST_F(EZMQPublisherTest, publish)
{
    ezmq::Event event = getProtoBufEvent();
    EXPECT_EQ(EZMQ_OK, mPublisher->start());
    EXPECT_EQ(EZMQ_OK, mPublisher->publish(event));
}


TEST_F(EZMQPublisherTest, publishOnTopic)
{
    ezmq::Event event = getProtoBufEvent();
    EXPECT_EQ(EZMQ_OK, mPublisher->start());
    EXPECT_EQ(EZMQ_OK, mPublisher->publish(mTopic, event));
}

TEST_F(EZMQPublisherTest, publishOnTopic1)
{
    EXPECT_EQ(EZMQ_OK, mPublisher->start());
    ezmq::Event event = getProtoBufEvent();

    // Empty topic test
    std::string testingTopic = "";
    EXPECT_EQ(EZMQ_INVALID_TOPIC, mPublisher->publish(testingTopic, event));

    // Numeric test
    testingTopic = "123";
    EXPECT_EQ(EZMQ_OK, mPublisher->publish(testingTopic, event));

    // Alpha-Numeric test
    testingTopic = "1a2b3";
    EXPECT_EQ(EZMQ_OK, mPublisher->publish(testingTopic, event));

    // Alphabet forward slash test
    testingTopic = "topic/";
    EXPECT_EQ(EZMQ_OK, mPublisher->publish(testingTopic, event));

    // Alphabet-Numeric, forward slash test
    testingTopic = "topic/13/4jtjos/";
    EXPECT_EQ(EZMQ_OK, mPublisher->publish(testingTopic, event));

    // Alphabet-Numeric, forward slash test
    testingTopic = "123a/1this3/4jtjos";
    EXPECT_EQ(EZMQ_OK, mPublisher->publish(testingTopic, event));

    // Topic contain forward slash at last
    testingTopic = "topic/122/livingroom/";
    EXPECT_EQ(EZMQ_OK, mPublisher->publish(testingTopic, event));

    // Topic contain -
    testingTopic = "topic/122/livingroom/-";
    EXPECT_EQ(EZMQ_OK, mPublisher->publish(testingTopic, event));

    // Topic contain _
    testingTopic = "topic/122/livingroom_";
    EXPECT_EQ(EZMQ_OK, mPublisher->publish(testingTopic, event));

    // Topic contain .
    testingTopic = "topic/122.livingroom.";
    EXPECT_EQ(EZMQ_OK, mPublisher->publish(testingTopic, event));
}

TEST_F(EZMQPublisherTest, publishNegative)
{
    EXPECT_EQ(EZMQ_OK, mPublisher->start());
    ezmq::Event event = getProtoBufEvent();
    EXPECT_EQ(EZMQ_INVALID_TOPIC, mPublisher->publish("", event));

    std::list<std::string> topicList;
    EXPECT_EQ(EZMQ_INVALID_TOPIC, mPublisher->publish(topicList, event));

    topicList.push_back("topic1");
    topicList.push_back("");
    EXPECT_EQ(EZMQ_INVALID_TOPIC, mPublisher->publish(topicList, event));

    topicList.clear();
    topicList.push_back("topic1");
    topicList.push_back("topic2");
    EXPECT_EQ(EZMQ_OK, mPublisher->publish(topicList, event));
}

TEST_F(EZMQPublisherTest, getPort)
{
    EXPECT_EQ(mPort, mPublisher->getPort());
}