#include "NodeStatusReport.h"
#include "dds/dds.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

#define DOMAIN_ID 0
#define TOPIC_NAME "NodeStatusReport"

int main()
{
    dds_entity_t participant;
    dds_entity_t topic;
    dds_entity_t writer;
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

    NodeStatusReport data;
    memset(&data, 0, sizeof(data));
    data.node_name = "CSP1";  // 字符串常量，没问题

    printf("Starting Publisher...\n");

    while (1) {
        data.cpu_a_usage = rand() % 10000;
        data.cpu_m_usage = rand() % 10000;
        data.ddr_usage = 512 + (rand() % 100);
        data.total_ddr = 1024;
        data.timestamp = (uint64_t)time(NULL);

        rc = dds_write(writer, &data);
        if (rc != DDS_RETCODE_OK) {
            fprintf(stderr, "Failed to write sample: %s\n", dds_strretcode(-rc));
            break;  // Conse加：写失败，退出
        } else {
            printf("Published: A:%u%% M:%u%% DDR:%uMB\n", 
                data.cpu_a_usage / 100, data.cpu_m_usage / 100, data.ddr_usage);
        }

        sleep(1);
    }

    dds_delete(participant);
    return EXIT_SUCCESS;
}
