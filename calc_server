#include <stdio.h>
#include <stdlib.h>
#include <process.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/neutrino.h>

#include "struct.h"

int main(void) {

	int rcvid;        // indicates who we should reply to
	int chid;         // This is Channel ID

	client_send_t msg;
	server_response_t resMsg;
	char message[50];

	// Create a channel
	chid = ChannelCreate(0);
	if (chid == -1) {
		perror("failed to create the channel.");
		exit(EXIT_FAILURE);
	}

	printf("Server PID is %d and the Channel ID (chid) is: %d\n", getpid(),
			chid);

	while (1) {

		//Get The Message
		rcvid = MsgReceive(chid, &msg, sizeof(msg), NULL);

		//Store values in temp double variables to receive right precision and then store in the Server response struct
		double tempLeft = msg.left_hand;
		double tempRight = msg.right_hand;
		switch (msg.operator) {
		case '+':
			resMsg.answer = (tempLeft + tempRight);
			break;
		case '-':
			resMsg.answer = (tempLeft - tempRight);
			break;
		case '*':
			resMsg.answer = (tempLeft * tempRight);
			break;
		case '/':
			resMsg.answer = (tempLeft / tempRight);
			break;
		}

		// Creating the reply
		snprintf(message, 50, "%.2f", resMsg.answer);

		//Reply, client will 'REPLY' block on server
		MsgReply(rcvid, EOK, message, sizeof(message));
	}

	// Destroy Channel When Finished
	ChannelDestroy(chid);
}
