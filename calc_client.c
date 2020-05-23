#include <stdio.h>
#include <stdlib.h>
#include <process.h>
#include <sys/neutrino.h>
#include <sys/netmgr.h>
#include <limits.h>
#include "../../calc_server/src/struct.h"

int main(int argc, char* argv[]) {

	// Data Type Created To Send Messages
	client_send_t smsg;

	//Data Type for Server Response
	server_response_t rmsg;

	// Used for Establishing The Connection
	int coid;

	//Error Checking to make sure the right ammount of Command-Line Arguments have been entered
	if (argc < 4) {
		printf(
				"Usage: ./calc_client <Calc-Server-PID> left_operand right_operand");
		exit(EXIT_FAILURE);
	}

	//1st Command Line Argument -- ServerPID
	pid_t serverpid = atoi(argv[1]);

	// 2nd Command Line Argument -- Left Hand Argument
	smsg.left_hand = atoi(argv[2]);

	// 4th Command-Line Argument - Right Hand Argument
	smsg.right_hand = atoi(argv[4]);

	// If Operator = x Change to *
	if (*argv[3] == 'x') {
		smsg.operator = '*';
	} else {
		smsg.operator = *argv[3];
	}

	// Establish the Connection
	coid = ConnectAttach(ND_LOCAL_NODE, serverpid, 1, _NTO_SIDE_CHANNEL, 0);
	if (coid == -1) {
		fprintf(stderr, "Couldn't ConnectAttach\n");
		exit(EXIT_FAILURE);
	}

	//Send The Message
	if (MsgSend(coid, &smsg, sizeof(smsg) + 1, &rmsg.answer,
			sizeof(rmsg.answer)) == -1) {
		printf("MsgSend Had An Error\n");
		exit(EXIT_FAILURE);
	}

	// Error Testing For Overflow
	if (smsg.left_hand > 0 && smsg.right_hand > INT_MAX - smsg.left_hand) {

		rmsg.statusCode = SRVR_OVERFLOW;

		snprintf(rmsg.errorMsg, sizeof(rmsg.errorMsg),
				"Error message from server: OVERFLOW %d %c %d", smsg.left_hand,
				smsg.operator, smsg.right_hand);
	}



	// Error Testing for Two Divide By 0
	if ((smsg.right_hand == 0) && (smsg.left_hand == 0)
			&& (smsg.operator == '/')) {

		rmsg.statusCode = SRVR_UNDEFINED;
		rmsg.answer = 0;
	}

	// Error Testing for Divide By 0
	if ((smsg.right_hand == 0) && (smsg.left_hand != 0)
			&& (smsg.operator == '/')) {

		rmsg.statusCode = SRVR_UNDEFINED;

		snprintf(rmsg.errorMsg, sizeof(rmsg.errorMsg),
				"Error message from server: UNDEFINED %d %c %d", smsg.left_hand,
				smsg.operator, smsg.right_hand);

	}

	// Error Testing For Random Operator
	if (smsg.operator != '/' && smsg.operator != '*' && smsg.operator != '+'
			&& smsg.operator != '-') {
		rmsg.statusCode = SRVR_INVALID_OPERATOR;

		snprintf(rmsg.errorMsg, sizeof(rmsg.errorMsg),
				"Error message from server: INVALID OPERATOR  %c ",
				smsg.operator);

	}

	//Either Print the Error Message or The Results
	if (rmsg.errorMsg[0] == '\0') {
		rmsg.statusCode = SRVR_OK;
		printf("The server has calculated the result of %d %c %d ",
				smsg.left_hand, smsg.operator, smsg.right_hand);
		printf("as %s\n", &rmsg.answer);
	} else {
		printf("%s", rmsg.errorMsg);
	}

	//Disconnect from the channel
	ConnectDetach(coid);
	return EXIT_SUCCESS;
}
