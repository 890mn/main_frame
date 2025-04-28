#include "NodeStatusReport.h"
#include "dds/dds.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> // for sleep()

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

    // 先等待至少有一个Publisher匹配上
    printf("Waiting for publisher to match...\n");

    dds_instance_handle_t matched;
    int wait_count = 0;
    while (1) {
        rc = dds_get_matched_publications(reader, &matched, 1);
        if (rc < 0) {
            fprintf(stderr, "dds_get_matched_publications failed: %s\n", dds_strretcode(-rc));
            dds_delete(participant);
            return EXIT_FAILURE;
        }
        if (rc > 0) {
            printf("Publisher matched! Ready to receive data.\n");
            break;
        }

        wait_count++;
        if (wait_count % 5 == 0) {
            printf("Still waiting for publisher...\n");
        }
        usleep(200 * 1000); // Conse调参：每200ms检查一次，比较温柔
    }

    NodeStatusReport data;
    void* samples[1] = { NULL };
    dds_sample_info_t infos[1];

    while (1) {
        samples[0] = &data;
        memset(&infos[0], 0, sizeof(infos[0]));

        rc = dds_take(reader, samples, infos, 1, 1);
        if (rc < 0) {
            fprintf(stderr, "dds_take failed: %s\n", dds_strretcode(-rc));
            break;
        }
        if (rc > 0 && samples[0] != NULL && infos[0].valid_data) {
            NodeStatusReport *recv = (NodeStatusReport *)samples[0];
            printf("[Recv] Node: %s | CPU_A: %u%% | CPU_M: %u%% | DDR: %uMB/%uMB | TS: %lu\n",
                recv->node_name,
                recv->cpu_a_usage / 100,
                recv->cpu_m_usage / 100,
                recv->ddr_usage,
                recv->total_ddr,
                recv->timestamp);
        }
        usleep(50 * 1000); // Conse加点小延时，降低空转负担
    }

    dds_delete(participant);
    return EXIT_SUCCESS;
}
