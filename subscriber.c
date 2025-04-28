#include "NodeStatusReport.h"
#include "dds/dds.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DOMAIN_ID 0
#define TOPIC_NAME "NodeStatusReport"
#define DDS_ENTITY_INVALID (-1)

int main()
{
    dds_entity_t participant = DDS_ENTITY_INVALID;
    dds_entity_t topic = DDS_ENTITY_INVALID;
    dds_entity_t reader = DDS_ENTITY_INVALID;
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

    reader = dds_create_reader(participant, topic, NULL, NULL);
    if (reader < 0) {
        fprintf(stderr, "Failed to create reader: %s\n", dds_strretcode(-reader));
        dds_delete(participant);
        return EXIT_FAILURE;
    }

    printf("Starting Subscriber...\n");

    NodeStatusReport *data = NULL;
    void* samples[1] = { NULL };
    dds_sample_info_t infos[1];

    // Allocate memory for the sample
    data = NodeStatusReport__alloc();
    if (data == NULL) {
        fprintf(stderr, "Failed to allocate memory for sample\n");
        dds_delete(participant);
        return EXIT_FAILURE;
    }
    samples[0] = data;

    while (1) {
        rc = dds_take(reader, samples, infos, 1, 1);
        if (rc < 0) {
            fprintf(stderr, "dds_take failed: %s\n", dds_strretcode(-rc));
            break;
        }
        if (rc > 0 && infos[0].valid_data) {
            NodeStatusReport *recv = samples[0];
            printf("[Recv] Node: %s | CPU_A: %u%% | CPU_M: %u%% | DDR: %uMB/%uMB | TS: %lu\n",
                recv->node_name, 
                recv->cpu_a_usage / 100, 
                recv->cpu_m_usage / 100, 
                recv->ddr_usage, 
                recv->total_ddr, 
                recv->timestamp);
        }
        // Return loan after processing
        dds_return_loan(reader, samples, rc);
    }

    // Free allocated memory
    NodeStatusReport_free(data, DDS_FREE_ALL);

    // Cleanup: Ensure deleting entities correctly
    if (reader != DDS_ENTITY_INVALID) {
        dds_delete(reader);
    }
    if (topic != DDS_ENTITY_INVALID) {
        dds_delete(topic);
    }
    if (participant != DDS_ENTITY_INVALID) {
        dds_delete(participant);
    }

    return EXIT_SUCCESS;
}
