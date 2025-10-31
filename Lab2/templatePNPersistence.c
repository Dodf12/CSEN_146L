//COEN 146L : Lab2, step 4
#include <stdlib.h>
#include <stdio.h>

int main(int argc, char *argv[]) 
{
        if (argc != 2) {  // check correct usage
		fprintf(stderr, "usage: %s <n connections> \n", argv[0]);
		exit(1);
	}
        //Declare and values to n (n connections), np (np parralel connections), RTT0, RTT1, RTT2, RTTHTTP, RTTDNS, .. 
        int n = atoi(argv[1]);
        int np;

        /*
        * Name: Abhinav
        * Date: 9-30-25
        * Title: Lab2 – template switch
        * Description: This shows the difference of runtime between persistent and non-persistent HTTP
        */

        int RTT0, RTT1, RTT2, RTTHTTP;
        RTT0 = 3;
        RTT1 =  20;
        RTT2 = 26;
        RTTHTTP = 47;
        
        int oneObjTime = RTT0 + RTT1 + RTT2 + 2 * RTTHTTP;
        int sixObjects = RTT0 + RTT1 + RTT2 + 2*RTTHTTP + 2  * 6 * RTTHTTP;

        if (n < 6) {np = 1;}
        else { np = 6 / n;}

        
        printf("One object: %d msec\n", oneObjTime);
        printf("Non-Persistent 6 objects: %d msec\n", sixObjects);

        //find how many np (parralel connections)

        int RTTDNS = RTT0 + RTT1 + RTT2;
        int persistent = RTTDNS + 2* (RTTHTTP )+ np * RTTHTTP;
        // double temp = RTTDNS +2* (RTTHTTP );
        // print("Part of persistant\n", temp );
        int nonPersistent = RTTDNS + 2 * RTTHTTP + 2 * np * RTTHTTP;
        
        printf("%d parallel connection - Persistent: %d msec\n", n, persistent);
        printf("%d parallel connection - Non-Persistent: %d msec\n", n ,nonPersistent);

return 0;
}