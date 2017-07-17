#include <sys/types.h>
#include <sys/systm.h>
#include <sys/param.h>
#include <sys/conf.h>
#include <sys/kernel.h>
#include <sys/module.h>
#include <machine/cpu.h>

/* For use with destroy_dev(9). */
static struct cdev *kwatchdog_dev;

static struct callout my_c;

void kwatchdog_wip (void*);
static int kwatcher_count_critical = 60;
int kwatcher_count = 0;

static struct cdevsw kwatchdog_cdevsw = {
	.d_version =    D_VERSION,
	.d_name =       "kwatchdog",
};

/* The function called at load/unload. */
static int event_handler(struct module *module, int event, void *arg) {
	int e = 0; /* Error, 0 for normal return status */
	int tmp = 0;
	switch (event) {
	case MOD_LOAD:
		callout_init(&my_c, 1);
		kwatchdog_dev = make_dev(&kwatchdog_cdevsw, 0,UID_ROOT, GID_WHEEL, 0600, "kwatchdog");
		uprintf("kwatchdog enabled! hz = %d, tick = %d\n", hz, tick);

		if (callout_pending(&my_c)){
			printf("kwatchdog callout: pending\n");
		}
		if (callout_active(&my_c)){
			printf("kwatchdog callout: active and armed!\n");
		}

		printf("kwatchdog callout reseting!\n");
		callout_reset(&my_c, 5*hz, kwatchdog_wip, NULL);
		printf("kwatchdog after reseting\n");

		TUNABLE_INT_FETCH("kwatchdog.count.critical", &tmp);
		if (tmp !=0) {
			kwatcher_count_critical = tmp;
		}
		break;
	case MOD_UNLOAD:
		callout_deactivate(&my_c);
		callout_stop(&my_c);
		callout_drain(&my_c);
		destroy_dev(kwatchdog_dev);
		uprintf("kwatchdog disarmament!\n");
		break;
	case MOD_SHUTDOWN:
		return 0;
	default:
		e = EOPNOTSUPP; /* Error, Operation Not Supported */
		break;
	}
	return(e);
}

void
kwatchdog_wip(void *arg)
{
	kwatcher_count++;

	if (callout_pending(&my_c)){
		printf("kwatchdog in progress: callout pending: %d/%d\n",kwatcher_count,kwatcher_count_critical);
	}
	if (callout_active(&my_c)){
		printf("kwatchdog in progress: callout active: %d/%d\n",kwatcher_count,kwatcher_count_critical);
	}
	if (kwatcher_count < kwatcher_count_critical ){
		callout_reset(&my_c, 5*hz, kwatchdog_wip, NULL);
	} else {
		cpu_reset();
	}
}

DEV_MODULE(mywatcg, event_handler, NULL);
