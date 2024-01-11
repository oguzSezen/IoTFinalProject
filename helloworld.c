

/* PMSIS includes */

#include "pmsis.h"
#include "NN_example.h"




/* Task executed by cluster cores. */

void cluster_helloworld(void *arg)

{

	
    uint32_t core_id = pi_core_id(), cluster_id = pi_cluster_id();

    if (core_id == 0) {

	printf("\n[%d %d] Task A from Core %d\n", cluster_id, core_id, core_id);
	initialize();
	imain();
	printf("\nInitialize is completed!n");


    } else if (core_id == 1) {
    
    	printf("\n[%d %d] Task B from Core %d\n", cluster_id, core_id, core_id);
	
	printf("\nmain completed");		
        
    }
    else if (core_id == 2) {

	printf("\n[%d %d] Task C from Core %d\n", cluster_id, core_id, core_id);

    }
    /*else if (core_id == 3) {

        
    }
    else if (core_id == 4) {

        
    }*/
    
}



/* Cluster main entry, executed by core 0. */

void cluster_delegate(void *arg)

{

    printf("Cluster master core entry\n");

    /* Task dispatch to cluster cores. */
    


    pi_cl_team_fork(pi_cl_cluster_nb_cores(), cluster_helloworld, arg);//

    printf("Cluster master core exit\n");

}



void helloworld(void)

{

    printf("Entering main controller\n");



    uint32_t errors = 0;

    uint32_t core_id = pi_core_id(), cluster_id = pi_cluster_id();

    printf("[0x%x %d] Hello World!\n", cluster_id, core_id);



    struct pi_device cluster_dev;

    struct pi_cluster_conf cl_conf;



    /* Init cluster configuration structure. */

    pi_cluster_conf_init(&cl_conf);

    cl_conf.id = 0;                /* Set cluster ID. */

    /* Configure & open cluster. */

    pi_open_from_conf(&cluster_dev, &cl_conf);



    pi_perf_conf(1 << PI_PERF_CYCLES | 1 << PI_PERF_ACTIVE_CYCLES| 1 << PI_PERF_LD| 1 <<PI_PERF_ST );

    pi_perf_reset();

    pi_perf_start();



    if (pi_cluster_open(&cluster_dev))

    {

        printf("Cluster open failed !\n");

        pmsis_exit(-1);

    }



    pi_perf_stop();

    uint32_t cycles = pi_perf_read(PI_PERF_ACTIVE_CYCLES);

    uint32_t tim_cycles = pi_perf_read(PI_PERF_CYCLES);

    

    uint32_t load_times = pi_perf_read(PI_PERF_LD); //important

    uint32_t storage_times = pi_perf_read(PI_PERF_ST); //important

    

    printf(" %d cycles for processor\n %d cycles for code execution\n", cycles, tim_cycles);//important

    printf(" %d loading operation\n %d for store operation\n", load_times, storage_times);//important

    

    

    

    //printf("Perf : %d cycles Timer : %d cycles\n", cycles, tim_cycles);

   



    /* Prepare cluster task and send it to cluster. */

    struct pi_cluster_task cl_task;



    pi_cluster_send_task_to_cl(&cluster_dev, pi_cluster_task(&cl_task, cluster_delegate, NULL));



    pi_cluster_close(&cluster_dev);



    printf("Test success !\n");



    pmsis_exit(errors);

}



/* Program Entry. */

int main(void)

{

    printf("\n\n\t *** PMSIS HelloWorld ***\n\n");

    return pmsis_kickoff((void *) helloworld);

}
