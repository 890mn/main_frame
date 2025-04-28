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
        printf("Failed to create participant\n");
        return -1;
    }

    topic = dds_create_topic(
        participant, &NodeStatusReport_desc, TOPIC_NAME, NULL, NULL);

    writer = dds_create_writer(participant, topic, NULL, NULL);

    NodeStatusReport data;
    memset(&data, 0, sizeof(data));

    data.node_name = "CSP1";

    printf("Starting Publisher...\n");
    while (1) {
        data.cpu_a_usage = rand() % 10000; // 模拟 0%~100%
        data.cpu_m_usage = rand() % 10000;
        data.ddr_usage = 512 + (rand() % 100); // 512~612MB
        data.total_ddr = 1024;
        data.timestamp = (uint64_t)time(NULL);

        rc = dds_write(writer, &data);
        if (rc != DDS_RETCODE_OK) {
            printf("Failed to write sample: %d\n", rc);
        } else {
            printf("Published: A:%u%% M:%u%% DDR:%uMB\n", 
                data.cpu_a_usage / 100, data.cpu_m_usage / 100, data.ddr_usage);
        }

        sleep(1); // 每秒发一次
    }

    dds_delete(participant);
    return 0;
}

