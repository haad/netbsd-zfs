
#include <sys/cdefs.h>

#include <sys/mount.h>
#include <sys/param.h>
#include <sys/kernel.h>
#include <sys/systm.h>
#include <sys/misc.h>
#include <sys/sunddi.h>
#include <sys/utsname.h>
#include <sys/vnode.h>
#include <sys/mount.h>
#include <sys/pool.h>
#include <sys/buf.h>

static void
vn_rele_inactive(vnode_t *vp)
{

	vrele(vp);
}

/*
 * Like vn_rele() except if we are going to call VOP_INACTIVE() then do it
 * asynchronously using a taskq. This can avoid deadlocks caused by re-entering
 * the file system as a result of releasing the vnode. Note, file systems
 * already have to handle the race where the vnode is incremented before the
 * inactive routine is called and does its locking.
 *
 * Warning: Excessive use of this routine can lead to performance problems.
 * This is because taskqs throttle back allocation if too many are created.
 */
void
vn_rele_async(vnode_t *vp, taskq_t *taskq)
{
	VERIFY(vp->v_count > 0);
	
	VI_LOCK(vp);
	if (vp->v_count == 1 && !(vp->v_iflag & VI_INACTNOW)) {
		VI_UNLOCK(vp);
		VERIFY(taskq_dispatch((taskq_t *)taskq,
			(task_func_t *)vn_rele_inactive, vp, TQ_SLEEP) != 0);
		return;
	} 

	atomic_dec_int(&vp->v_usecount, 1);
}
