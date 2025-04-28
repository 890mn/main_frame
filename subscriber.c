#include "NodeStatusReport.h"
#include "dds/dds.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DOMAIN_ID 0
#define TOPIC_NAME "NodeStatusReport"

int main()
{
    dds_entity_t participant;
    dds_entity_t topic;
    dds_entity_t reader;
    dds_return_t rc;

    participant = dds_create_participant(DOMAIN_ID, NULL, NULL);
    if (participant < 0) {
        printf("Failed to create participant\n");
        return -1;
    }

    topic = dds_create_topic(
        participant, &NodeStatusReport_desc, TOPIC_NAME, NULL, NULL);

    reader = dds_create_reader(participant, topic, NULL, NULL);

    printf("Starting Subscriber...\n");

    NodeStatusReport data;
    void* samples[1];
    dds_sample_info_t infos[1];
    samples[0] = &data;

    while (1) {
        rc = dds_take(reader, samples, infos, 1, 1);
        if (rc > 0 && infos[0].valid_data) {
            printf("[Recv] Node: %s | CPU_A: %u%% | CPU_M: %u%% | DDR: %uMB/%uMB | TS: %lu\n",
                data.node_name, 
                data.cpu_a_usage / 100, 
                data.cpu_m_usage / 100, 
                data.ddr_usage, 
                data.total_ddr, 
                data.timestamp);
        }
    }

    dds_delete(participant);
    return 0;
}

