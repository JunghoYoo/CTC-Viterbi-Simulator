#pragma once

const int   TBCC_CODE_RATE = 3;
const int   TBCC_CONSTRAINT_LEN = 7;
const int   TBCC_NUM_STATES = (1 << (TBCC_CONSTRAINT_LEN - 1));
const int   TBCC_TRACEBACK_LEN = TBCC_CONSTRAINT_LEN * 6;
const int	TBCC_G[TBCC_CODE_RATE] = { 133, 171, 165 };
const int	TBCC_ITERATIONFORINITIALSTATE = 3;