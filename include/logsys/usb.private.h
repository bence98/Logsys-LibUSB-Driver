#ifndef _LOGSYSDRV_USB_PRIVATE_H
#define _LOGSYSDRV_USB_PRIVATE_H

/* interface numbers */
#define LOGSYS_IFN_DEV 0
#define LOGSYS_IFN_COM 1
/* endpoints: - interface0 */
/* non-interrupting function endpoints */
#define LOGSYS_OUT_EP1 0x01
#define LOGSYS_IN_EP2  0x82
/* interrupting function endpoints */
#define LOGSYS_OUT_EP3 0x03
#define LOGSYS_IN_EP4  0x84
/* - interface1 */
/* COM endpoints */
#define LOGSYS_OUT_EP5 0x05
#define LOGSYS_IN_EP6  0x86

#define LOGSYS_REQTYP_IN  0xc0
#define LOGSYS_REQTYP_OUT 0x40

#endif //_LOGSYSDRV_USB_PRIVATE_H
