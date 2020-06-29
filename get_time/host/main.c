
#include <err.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
/* OP-TEE TEE client API (built by optee_client) */
#include <tee_client_api.h>

/* To the the UUID (found the the TA's h-file(s)) */
#include <get_time_ta.h>

int main(int argc, char *argv[])
{
	int choice;
	if (argc < 3) {
		printf("Usage: %s get_time 0|1 (0 -> system time | 1 -> REE time) \n", argv[0]);
		exit(EXIT_FAILURE);
	}
 	choice = atoi(argv[1]); 
	if (choice != 0 && choice != 1) {
		printf("Choice must be 0 or 1 only!!!\n");
		exit(EXIT_FAILURE);
	}
	TEEC_Result res;
	TEEC_Context ctx;
	TEEC_Session sess;
	TEEC_Operation op;
	TEEC_UUID uuid = TA_GET_TIME_UUID;
	uint32_t err_origin;
	/* Initialize a context connecting us to the TEE */
	res = TEEC_InitializeContext(NULL, &ctx);
	if (res != TEEC_SUCCESS)
		errx(1, "TEEC_InitializeContext failed with code 0x%x", res);

	/*
	 * Open a session to the "hello world" TA, the TA will print "hello
	 * world!" in the log when the session is created.
	 */
	res = TEEC_OpenSession(&ctx, &sess, &uuid,
			       TEEC_LOGIN_PUBLIC, NULL, NULL, &err_origin);
	if (res != TEEC_SUCCESS)
		errx(1, "TEEC_Opensession failed with code 0x%x origin 0x%x",
			res, err_origin);

	/*
	 * Execute a function in the TA by invoking it, in this case
	 * we're incrementing a number.
	 *
	 * The value of command ID part and how the parameters are
	 * interpreted is part of the interface provided by the TA.
	 */

	/* Clear the TEEC_Operation struct */
	memset(&op, 0, sizeof(op));

	/*
	 * Prepare the argument. Pass a value in the first parameter,
	 * the remaining three parameters are unused.
	 */
	op.paramTypes = TEEC_PARAM_TYPES(TEEC_VALUE_INOUT, TEEC_NONE,
					 TEEC_NONE, TEEC_NONE);
	

	switch(choice){
	case 1 :
		res = TEEC_InvokeCommand(&sess, TA_CMD_GET_REE_TIME, &op,
					&err_origin);
		if (res != TEEC_SUCCESS)
			errx(1, "TEEC_InvokeCommand failed with code 0x%x origin 0x%x",
				res, err_origin);
		printf("The REE time is %d\n", op.params[0].value.a);
		printf("The REE time in millis : %d\n", op.params[0].value.b);
	case 0 :
		res = TEEC_InvokeCommand(&sess, TA_CMD_GET_SYSTEM_TIME, &op,
					&err_origin);
		if (res != TEEC_SUCCESS)
			errx(1, "TEEC_InvokeCommand failed with code 0x%x origin 0x%x",
				res, err_origin);
	    printf("The system time in second : %d\n", op.params[0].value.a);
		printf("The system time in millis : %d\n", op.params[0].value.b);
	
	
	}
	/*
	 * We're done with the TA, close the session and
	 * destroy the context.
	 *
	 * The TA will print "Goodbye!" in the log when the
	 * session is closed.
	 */

	TEEC_CloseSession(&sess);

	TEEC_FinalizeContext(&ctx);

	return 0;
}
