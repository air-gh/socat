/* source: xio-interface.c */
/* Copyright Gerhard Rieger 2010 */
/* Published under the GNU General Public License V.2, see file COPYING */

/* this file contains the source for opening addresses of raw socket type */

#include "xiosysincludes.h"

#if WITH_INTERFACE

#include "xioopen.h"
#include "xio-socket.h"

#include "xio-interface.h"


static
int xioopen_interface(int argc, const char *argv[], struct opt *opts,
		      int xioflags, xiofile_t *xfd, unsigned groups, int pf,
		      int dummy2, int dummy3);

const struct addrdesc xioaddr_interface= { "interface",    3, xioopen_interface, GROUP_FD|GROUP_SOCKET, PF_PACKET, 0, 0 HELP(":<interface>") };


static
int _xioopen_interface(const char *ifname,
		       struct opt *opts, int xioflags, xiofile_t *xxfd,
		       unsigned groups, int pf) {
   xiosingle_t *xfd = &xxfd->stream;
   union sockaddr_union us = {{0}};
   socklen_t uslen;
   int socktype = SOCK_RAW;
   unsigned int ifidx;
   bool needbind = false;
   char *bindstring = NULL;
   struct sockaddr_ll sall = { 0 };

   if (ifindex(ifname, &ifidx, -1) < 0) {
      Error1("unknown interface \"%s\"", ifname);
      ifidx = 0;	/* desparate attempt to continue */
   }

   xfd->howtoend = END_SHUTDOWN;
   retropt_int(opts, OPT_SO_TYPE, &socktype);

   retropt_socket_pf(opts, &pf);

   /* ...res_opts[] */
   if (applyopts_single(xfd, opts, PH_INIT) < 0)  return -1;
   applyopts(-1, opts, PH_INIT);

   xfd->salen = sizeof(xfd->peersa);
   if (pf == PF_UNSPEC) {
      pf = xfd->peersa.soa.sa_family;
   }

   xfd->dtype = XIODATA_RECVFROM_SKIPIP;

   if (retropt_string(opts, OPT_BIND, &bindstring)) {
      needbind = true;
   }
   /*!!! parse by ':' */
   us.ll.sll_family = pf;
   us.ll.sll_protocol = htons(ETH_P_ALL);
   us.ll.sll_ifindex = ifidx;
   uslen = sizeof(sall);
   needbind = true;
   xfd->peersa = (union sockaddr_union)us;

   return
      _xioopen_dgram_sendto(needbind?&us:NULL, uslen,
			  opts, xioflags, xfd, groups, pf, socktype, 0);
}

static
int xioopen_interface(int argc, const char *argv[], struct opt *opts,
		      int xioflags, xiofile_t *xxfd, unsigned groups,
		      int pf, int dummy2, int dummy3) {
   xiosingle_t *xfd = &xxfd->stream;
   int result;

   if (argc != 2) {
      Error2("%s: wrong number of parameters (%d instead of 1)",
	     argv[0], argc-1);
      return STAT_NORETRY;
   }

   if ((result =
	_xioopen_interface(argv[1], opts, xioflags, xxfd, groups, pf))
       != STAT_OK) {
      return result;
   }

   xfd->dtype = XIOREAD_RECV|XIOWRITE_SENDTO;
   if (pf == PF_INET) {
      xfd->dtype |= XIOREAD_RECV_SKIPIP;
   }

   xfd->para.socket.la.soa.sa_family = xfd->peersa.soa.sa_family;

   _xio_openlate(xfd, opts);
   return STAT_OK;
}

#endif /* WITH_INTERFACE */
#if WITH_HCIINTERFACE



static
int xioopen_hciinterface(int argc, const char *argv[], struct opt *opts,
		      int xioflags, xiofile_t *xfd, unsigned groups, int pf,
		      int dummy2, int dummy3);

const struct addrdesc xioaddr_hciinterface= { "hciinterface",    3, xioopen_hciinterface, GROUP_FD|GROUP_SOCKET, PF_BLUETOOTH, 0, 0 HELP(":<hciinterface>") };


static
int _xioopen_hciinterface(const char *ifname,
		       struct opt *opts, int xioflags, xiofile_t *xxfd,
		       unsigned groups, int pf) {
   xiosingle_t *xfd = &xxfd->stream;
   union sockaddr_union us = {{0}};
   socklen_t uslen;
   int socktype = SOCK_RAW;
   unsigned int ifidx;
   bool needbind = false;
   char *bindstring = NULL;
   struct sockaddr_hci sahci = { 0 };

   if (strlen(ifname) != 4 || strncmp(ifname, "hci", 3) != 0 || *(ifname + 3) < '0' || '9' < *(ifname + 3)) {
      Error1("unknown hci interface \"%s\"", ifname);
      ifidx = 0;	/* desparate attempt to continue */
   } else {
      ifidx = *(ifname + 3) - '0';
   }

   xfd->howtoend = END_SHUTDOWN;
   retropt_int(opts, OPT_SO_TYPE, &socktype);

   retropt_socket_pf(opts, &pf);

   /* ...res_opts[] */
   if (applyopts_single(xfd, opts, PH_INIT) < 0)  return -1;
   applyopts(-1, opts, PH_INIT);

   xfd->salen = sizeof(xfd->peersa);
   if (pf == PF_UNSPEC) {
      pf = xfd->peersa.soa.sa_family;
   }

#if 0
   xfd->dtype = XIODATA_RECVFROM_SKIPIP;
#endif
   xfd->dtype = XIODATA_RECV|XIOWRITE_SENDTO;

   if (retropt_string(opts, OPT_BIND, &bindstring)) {
      needbind = true;
   }
   /*!!! parse by ':' */
   us.hci.hci_family = pf;
   us.hci.hci_dev = ifidx;
   us.hci.hci_channel = 0;
   uslen = sizeof(sahci);
   needbind = true; /* need for hci interface */
   xfd->peersa = (union sockaddr_union)us;

   return
      _xioopen_dgram_sendto(needbind?&us:NULL, uslen,
			    opts, xioflags, xfd, groups, pf, socktype, 1); /* 1 = BTPROTO_HCI */
}

static
int xioopen_hciinterface(int argc, const char *argv[], struct opt *opts,
		      int xioflags, xiofile_t *xxfd, unsigned groups,
		      int pf, int dummy2, int dummy3) {
   xiosingle_t *xfd = &xxfd->stream;
   int result;

   if (argc != 2) {
      Error2("%s: wrong number of parameters (%d instead of 1)",
	     argv[0], argc-1);
      return STAT_NORETRY;
   }

   if ((result =
	_xioopen_hciinterface(argv[1], opts, xioflags, xxfd, groups, pf))
       != STAT_OK) {
      return result;
   }

   xfd->dtype = XIOREAD_RECV|XIOWRITE_SENDTO;
   if (pf == PF_INET) {
      xfd->dtype |= XIOREAD_RECV_SKIPIP;
   }

   xfd->para.socket.la.soa.sa_family = xfd->peersa.soa.sa_family;

   _xio_openlate(xfd, opts);
   return STAT_OK;
}

#endif /* WITH_HCIINTERFACE */
