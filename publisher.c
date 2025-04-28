#include "NodeStatusReport.h"
#include "dds/dds.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

#define DOMAIN_ID 0
#define TOPIC_NAME "NodeStatusReport"
#define DDS_ENTITY_INVALID (-1)

int main()
{
    dds_entity_t participant = DDS_ENTITY_INVALID;
    dds_entity_t topic = DDS_ENTITY_INVALID;
    dds_entity_t writer = DDS_ENTITY_INVALID;
    dds_return_t rc;

    participant = dds_create_participant(DOMAIN_ID, NULL, NULL);
    if (participant < 0) {
        fprintf(stderr, "Failed to create participant: %s\n", dds_strretcode(-participant));
        return EXIT_FAILURE;
    }

    topic = dds_create_topic(
        participant, &NodeStatusReport_desc, TOPIC_NAME, NULL, NULL);
    if (topic < 0) {
        fprintf(stderr, "Failed to create topic: %s\n", dds_strretcode(-topic));
        dds_delete(participant);
        return EXIT_FAILURE;
    }

    writer = dds_create_writer(participant, topic, NULL, NULL);
    if (writer < 0) {
        fprintf(stderr, "Failed to create writer: %s\n", dds_strretcode(-writer));
        dds_delete(participant);
        return EXIT_FAILURE;
    }

    printf("Starting Publisher...\n");

    NodeStatusReport data;
    memset(&data, 0, sizeof(data));

    // Wait for subscriber to match
    dds_instance_handle_t matched;
    int wait_count = 0;
    while (1) {
        rc = dds_get_matched_subscriptions(writer, &matched, 1);
        if (rc < 0) {
            fprintf(stderr, "dds_get_matched_subscriptions failed: %s\n", dds_strretcode(-rc));
            break;
        }
        if (rc > 0) {
            printf("Subscriber matched! Ready to publish data.\n");
            break;
        }

        wait_count++;
        if (wait_count % 5 == 0) {
            printf("Still waiting for subscriber...\n");
        }
        usleep(200 * 1000);
    }

    srand((unsigned int)time(NULL)); 

    while (1) {
        data.cpu_a_usage = rand() % 10000;
        data.cpu_m_usage = rand() % 10000;
        data.ddr_usage = 512 + (rand() % 100);
        data.total_ddr = 1024;
        data.timestamp = (uint64_t)time(NULL);

        rc = dds_write(writer, &data);
        if (rc != DDS_RETCODE_OK) {
            fprintf(stderr, "Failed to write sample: %s\n", dds_strretcode(-rc));
            break;
        } else {
            printf("Published: Node:%s A:%u%% M:%u%% DDR:%u/%u MB @%lu\n",
                data.node_name,
                data.cpu_a_usage / 100,
                data.cpu_m_usage / 100,
                data.ddr_usage,
                data.total_ddr,
                data.timestamp);
        }

        sleep(1);
    }
    dds_free(data.node_name);

    // Cleanup: Ensure deleting entities correctly
    if (writer != DDS_ENTITY_INVALID) {
        dds_delete(writer);
    }
    if (topic != DDS_ENTITY_INVALID) {
        dds_delete(topic);
    }
    if (participant != DDS_ENTITY_INVALID) {
        dds_delete(participant);
    }

    return EXIT_SUCCESS;
}
