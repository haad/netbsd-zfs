#	$NetBSD: Makefile,v 1.1 2009/03/26 22:11:44 ad Exp $

.include <bsd.own.mk>

.if !defined(MKZFS) || ${MKZFS} != "no"
SUBDIR+=	zpool zfs
.endif

.include <bsd.subdir.mk>
