#include <stdio.h>

int main(int argc, char* argv[])
{
	printf("\t[ CPU Schedule Simulator ]\n\tUse 'h' to show possible commands.\n");
    printf("\tpossible commands:\n\tCMD\t\tACTION\n");

    printf("\n\tstart\t\tStart scheduling.\n");
    printf("\t\tUsage: start <options>\n");
    printf("\t\t\t<options> :\n");
    printf("\t\t\t-i\tInteraction mode – This let you to step through the changes using interactive shell.\n");
    printf("\t\t\t-n\tSchedule mode(default) – Use the selected schedule algorithm to make a gantt chart of the schedules.\n");

    printf("\n\tset\t\tSet the scheduling algorithm. This will be ignored on analysis.\n");
    printf("\t\tUsage: set <algorithm>\n");
    printf("\t\t\t<algorithm> :\n");
    printf("\t\t\tfcfs(default)\tFirst come first served algorithm.\n");
    printf("\t\t\tsjf\tShortest job first algorithm.\n");
    printf("\t\t\tsrtf\tShortest remaining time first algorithm\n");
    printf("\t\t\trr <time-quantum>\tRound robin algorithm. Insert an integer on time quantum.\n");
    printf("\t\t\tnppr\tNon-preemptive priority algorithm.\n");
    printf("\t\t\tppr\tPreemptive priority algorithm.\n");

    printf("\n\tgenerate\t\tGenerate a set of randomized processes and put them into the ready queue.\n");
    printf("\t\tUsage: generate <process-count>\n");
    printf("\t\t\t<process-count> : Count of processes to create. Positive integer.\n");

    printf("\n\tsave\t\tSave the current sessions as a file.\n");
    printf("\t\tUsage: save <file-name>\n");
    printf("\t\t\t<file-name> : Name of the session to save. Extension is \".cpus\".\n");

    printf("\n\tload\t\tLoad the saved session to analyze the session. The opened session will NOT be saved.\n");
    printf("\t\tUsage: load <file-name>\n");
    printf("\t\t\t<file-name> : Name of the session to load.\n");

    printf("\n\tanalyze\t\tAnalyze the session by testing all of the algorithm, and compare the output.\n");
    printf("\t\tUsage: analyze\n");

    printf("\n\n\n");



    return 0;
}

/*

			printf("\tpossible commands:\n\tCMD\t\tACTION\n");

			printf("\n\tadd\t\tAdd process to the process list.\n");
			printf("\t\tUsage: add <arrival time> <cpu burst time> <priority> [<io burst time 1> <io request time 1> ...]\n");

	        printf("\n\tstart\t\tStart scheduling.\n");
	        printf("\t\tUsage: start <options>\n");
	        printf("\t\t\t<options> :\n");
	        printf("\t\t\t-i\tInteraction mode – This let you to step through the changes using interactive shell.\n");
	        printf("\t\t\t-n\tSchedule mode(default) – Use the selected schedule algorithm to make a gantt chart of the schedules.\n");

	        printf("\n\tset\t\tSet the scheduling algorithm. This will be ignored on analysis.\n");
	        printf("\t\tUsage: set <algorithm>\n");
	        printf("\t\t\t<algorithm> :\n");
	        printf("\t\t\tfcfs(default)\tFirst come first served algorithm.\n");
	        printf("\t\t\tsjf\tShortest job first algorithm.\n");
	        printf("\t\t\tsrtf\tShortest remaining time first algorithm\n");
	        printf("\t\t\trr <time-quantum>\tRound robin algorithm. Insert an integer on time quantum.\n");
	        printf("\t\t\tnppr\tNon-preemptive priority algorithm.\n");
	        printf("\t\t\tppr\tPreemptive priority algorithm.\n");

	        printf("\n\tgenerate\t\tGenerate a set of randomized processes and put them into the ready queue.\n");
	        printf("\t\tUsage: generate <process-count>\n");
	        printf("\t\t\t<process-count> : Count of processes to create. Positive integer.\n");

	        printf("\n\tsave\t\tSave the current sessions as a file.\n");
	        printf("\t\tUsage: save <file-name>\n");
	        printf("\t\t\t<file-name> : Name of the session to save. Extension is \".cpus\".\n");

	        printf("\n\tload\t\tLoad the saved session to analyze the session. The opened session will NOT be saved.\n");
	        printf("\t\tUsage: load <file-name>\n");
	        printf("\t\t\t<file-name> : Name of the session to load.\n");

	        printf("\n\tanalyze\t\tAnalyze the session by testing all of the algorithm, and compare the output.\n");
	        printf("\t\tUsage: analyze\n");

			printf("\n\tquit\t\tQuit program.\n");
	        printf("\t\tUsage: quit\n");

			printf("\n\tadd\t\tAdd process to the process list.\n");
			printf("\n\tstart\t\tStart scheduling.\n");
			printf("\n\tset\t\tSet the scheduling algorithm. This will be ignored on analysis.\n");
			printf("\n\tgenerate\t\tGenerate a set of randomized processes and put them into the ready queue.\n");
			printf("\n\tsave\t\tSave the current sessions as a file.\n");
			printf("\n\tload\t\tLoad the saved session to analyze the session. The opened session will NOT be saved.\n");
			printf("\n\tanalyze\t\tAnalyze the session by testing all of the algorithm, and compare the output.\n");
			printf("\n\tquit\t\tQuit program.\n");
*/
