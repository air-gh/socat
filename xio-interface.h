/* source: xio-interface.h */
/* Copyright Gerhard Rieger 2008 */
/* Published under the GNU General Public License V.2, see file COPYING */

#ifndef __xio_interface_h_included
#define __xio_interface_h_included 1

extern const struct addrdesc xioaddr_interface;
#if WITH_HCIINTERFACE
extern const struct addrdesc xioaddr_hciinterface;
#endif /* WITH_HCIINTERFACE */

#endif /* !defined(__xio_interface_h_included) */
