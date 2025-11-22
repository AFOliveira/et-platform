/*-------------------------------------------------------------------------
* Copyright (c) 2025 Ainekko, Co.
* SPDX-License-Identifier: Apache-2.0
*-------------------------------------------------------------------------*/

// Thread, Hart and Shire ID utilities.

static inline bool shireid_is_ioshire(unsigned shireid)
{
    return (shireid == IO_SHIRE_ID);
}

static inline bool shireindex_is_ioshire(unsigned shireidx)
{
    return (shireidx == EMU_IO_SHIRE_SP);
}

static inline unsigned shireid(unsigned shireidx)
{
    return shireindex_is_ioshire(shireidx) ? IO_SHIRE_ID : shireidx;
}

static inline unsigned shireindex(unsigned shireid)
{
    return shireid_is_ioshire(shireid) ? EMU_IO_SHIRE_SP : shireid;
}

// Minion count per shire.
static inline unsigned shireindex_minions(unsigned shire)
{
    return shireindex_is_ioshire(shire) ? 1 : EMU_MINIONS_PER_SHIRE;
}

// Hart count per shire.
static inline unsigned shireindex_harts(unsigned shire)
{
    return shireindex_is_ioshire(shire) ? 1 : EMU_THREADS_PER_SHIRE;
}

// Neighborhood count per shire.
static inline unsigned shireindex_neighs(unsigned shire)
{
    return shireindex_is_ioshire(shire) ? 1 : EMU_NEIGH_PER_SHIRE;
}

// Harts per minion of a given shire.
static inline unsigned shireindex_minionharts(unsigned shire)
{
    return shireindex_is_ioshire(shire) ? 1 : EMU_THREADS_PER_MINION;
}

static inline bool hartid_is_svcproc(unsigned hartid)
{
    return hartid == IO_SHIRE_SP_HARTID;
}

static inline bool hartindex_is_svcproc(unsigned hartindex)
{
    return hartindex == EMU_IO_SHIRE_SP_THREAD;
}

static inline unsigned hartindex(unsigned hartid)
{
    return hartid_is_svcproc(hartid) ? EMU_IO_SHIRE_SP_THREAD : hartid;
}

static inline unsigned hartid(unsigned hartindex)
{
    return hartindex_is_svcproc(hartindex) ? IO_SHIRE_SP_HARTID : hartindex;
}
