#include "object.h"
#include "hdl4secell.h"

IHDL4SEUnit** hdl4seCreateDec2seg(IHDL4SEModule** parent, char* instanceparam, char* name) { /* module dec2seg */
	IHDL4SEUnit** wire_const[11];
	IHDL4SEUnit** unit_const[11];
	IHDL4SEUnit** unit_mux16 = NULL;
	IHDL4SEModule** module_dec2seg = NULL;
	IHDL4SEUnit** unit_dec2seg = NULL;
	int i;
	char temp[128];
	char* constparam[11] = {
		"8, 8'b00111111",
		"8, 8'b00000110",
		"8, 8'b01011011",
		"8, 8'b01001111",
		"8, 8'b01100110",
		"8, 8'b01101101",
		"8, 8'b01111101",
		"8, 8'b00000111",
		"8, 8'b01111111",
		"8, 8'b01101111",
		"8, 8'b01111001"
	};

	/* ����ģ����� */
	unit_dec2seg = hdl4seCreateUnit(parent, CLSID_HDL4SE_MODULE, instanceparam, name);
	/* �õ������IHDL4SEModule �ӿ� */
	objectQueryInterface(unit_dec2seg, IID_HDL4SEMODULE, &module_dec2seg);
	/* ���Ӷ˿� */
	objectCall3(module_dec2seg, AddPort, 4, PORTTYPE_INPUT,  "dec");
	objectCall3(module_dec2seg, AddPort, 8, PORTTYPE_OUTPUT, "seg");

	for (i = 0; i < 11; i++) {
		char tempname[32];
		sprintf(tempname, "wire_cst%d", i);
		wire_const[i] = hdl4seCreateUnit(module_dec2seg, CLSID_HDL4SE_WIRE, "8", tempname);
		sprintf(tempname, "const_cst%d", i);
		unit_const[i] = hdl4seCreateUnit(module_dec2seg, CLSID_HDL4SE_CONST, constparam[i], tempname);
		objectCall5(wire_const[i], Connect, 0, unit_const[i], 0, 0, 8);
	}
	/* ��������ѡ����unit_mux */
	unit_mux16 = hdl4seCreateUnit(module_dec2seg, CLSID_HDL4SE_MUX16, "8", "mux_dec");
	/*mux���������ӵ�����˿�dec������constall��*/
	objectCall5(unit_mux16, Connect, 0, unit_dec2seg, 0, 0, 4);
	for (i = 0; i < 10; i++) {
		objectCall5(unit_mux16, Connect, i+1, wire_const[i], 0, 0, 8);
	}
	for (; i < 16; i++) {
		objectCall5(unit_mux16, Connect, i + 11, wire_const[10], 0, 0, 8);
	}
	/* ����ģ������seg���ӵ����������������*/
	objectCall5(unit_dec2seg, Connect, 1, unit_mux16, 17, 0, 8);
	/*�ͷ�module�ӿ�*/
	objectRelease(module_dec2seg);
	/*����unit�ӿ�*/
	return unit_dec2seg;
}

IHDL4SEUnit** hdl4seCreateCounter(IHDL4SEModule** parent, char* instanceparam, char* name) { /* module counter */
	IHDL4SEModule** module_counter = NULL;
	IHDL4SEUnit** unit_counter = NULL;
	int width, maxvalue, resetvalue;
	char temp[128];
	sscanf(instanceparam, "%d, %d, %d", &width, &maxvalue, &resetvalue);
	/* ����ģ����� */
	unit_counter = hdl4seCreateUnit(parent, CLSID_HDL4SE_MODULE, instanceparam, name);
	/* �õ������IHDL4SEModule �ӿ� */
	objectQueryInterface(unit_counter, IID_HDL4SEMODULE, &module_counter);
	/* ���Ӷ˿� */
	objectCall4(module_counter, AddPort, 1, PORTTYPE_INPUT, 0, "wCounterIt");
	objectCall4(module_counter, AddPort, width, PORTTYPE_OUTPUT, 1, "bCouter");
	objectCall4(module_counter, AddPort, 1, PORTTYPE_OUTPUT, 2, "wCounterOverflow");

	/*WIDTH��ȵļĴ������������������ֵ*/
	sprintf(temp, "%d", width);
	IHDL4SEUnit** wire_wirein_bCurrentCounter = hdl4seCreateUnit(module_counter, CLSID_HDL4SE_WIRE, temp, "wirein_bCurrentCounter");
	IHDL4SEUnit** wire_wireout_bCurrentCounter = hdl4seCreateUnit(module_counter, CLSID_HDL4SE_WIRE, temp, "wireout_bCurrentCounter");
	IHDL4SEUnit** wire_wEn_bCurrentCounter = hdl4seCreateUnit(module_counter, CLSID_HDL4SE_WIRE, "1", "wEn_bCurrentCounter");
	sprintf(temp, "%d, %d", width, resetvalue);
	IHDL4SEUnit** reg_bCurrentCounter = hdl4seCreateUnit(module_counter, CLSID_HDL4SE_REG, temp, "bCurrentCounter");
	/*�Ĵ���������������һ��*/
	objectCall5(reg_bCurrentCounter, Connect, 0, wire_wEn_bCurrentCounter, 0, 0, 1);
	objectCall5(reg_bCurrentCounter, Connect, 1, wire_wirein_bCurrentCounter, 0, 0, width);
	objectCall5(wire_wireout_bCurrentCounter, Connect, 0, reg_bCurrentCounter, 2, 0, width);

	/*����һ���Ĵ�������ʾ�������Ƿ����*/

	IHDL4SEUnit** wire_wirein_wOverflow = hdl4seCreateUnit(module_counter, CLSID_HDL4SE_WIRE, "1", "wirein_wOverflow");
	IHDL4SEUnit** wire_wireout_wOverflow = hdl4seCreateUnit(module_counter, CLSID_HDL4SE_WIRE, "1", "wireout_wOverflow");
	IHDL4SEUnit** wire_wEn_wOverflow = hdl4seCreateUnit(module_counter, CLSID_HDL4SE_WIRE, "1", "wEn_wOverflow");
	IHDL4SEUnit** reg_wOverflow = hdl4seCreateUnit(module_counter, CLSID_HDL4SE_REG, "1, 0", "wOverflow");
	/*�Ĵ���������������һ��*/
	objectCall5(reg_wOverflow, Connect, 0, wire_wEn_wOverflow, 0, 0, 1);
	objectCall5(reg_wOverflow, Connect, 1, wire_wirein_wOverflow, 0, 0, 1);
	objectCall5(wire_wireout_wOverflow, Connect, 0, reg_wOverflow, 2, 0, 1);

	sprintf(temp, "%d", width);
	IHDL4SEUnit** wire_bCounter = hdl4seCreateUnit(module_counter, CLSID_HDL4SE_WIRE, temp, "bCounter");
	objectCall5(wire_bCounter, Connect, 0, wire_wireout_bCurrentCounter, 0, 0, width);
	IHDL4SEUnit** wire_wCounterOverflow = hdl4seCreateUnit(module_counter, CLSID_HDL4SE_WIRE, "1", "wCounterOverflow");
	objectCall5(wire_wCounterOverflow, Connect, 0, wire_wireout_wOverflow, 0, 0, 1);

	objectCall5(wire_wEn_bCurrentCounter, Connect, 0, unit_counter, 0, 0, 1);
	objectCall5(wire_wEn_wOverflow, Connect, 0, unit_counter, 0, 0, 1);

	sprintf(temp, "%d", width);
	IHDL4SEUnit** wire_bConst_MAXVALUE = hdl4seCreateUnit(module_counter, CLSID_HDL4SE_WIRE, temp, "bConst_MAXVALUE");
	sprintf(temp, "%d, %d", width, maxvalue);
	IHDL4SEUnit** unit_const_MAXVALUE = hdl4seCreateUnit(module_counter, CLSID_HDL4SE_CONST, temp, "const_MAXVALUE");
	objectCall5(wire_bConst_MAXVALUE, Connect, 0, unit_const_MAXVALUE, 0, 0, width);

	sprintf(temp, "%d", width);
	IHDL4SEUnit** wire_bConst_RESETVALUE = hdl4seCreateUnit(module_counter, CLSID_HDL4SE_WIRE, temp, "bConst_RESETVALUE");
	sprintf(temp, "%d, %d", width, resetvalue);
	IHDL4SEUnit** unit_const_RESETVALUE = hdl4seCreateUnit(module_counter, CLSID_HDL4SE_CONST, temp, "const_RESETVALUE");
	objectCall5(wire_bConst_RESETVALUE, Connect, 0, unit_const_RESETVALUE, 0, 0, width);

	IHDL4SEUnit** wire_wEQ_bCurrentCounter_MAXVALUE = hdl4seCreateUnit(module_counter, CLSID_HDL4SE_WIRE, "1", "wEQ_bCurrentCounter_MAXVALUE");

	sprintf(temp, "%d, %d, 1, %d", width, width, BINOP_EQ);
	IHDL4SEUnit** unit_binop_EQ_bCurrentCounter_MAXVALUE = hdl4seCreateUnit(module_counter, CLSID_HDL4SE_BINOP, temp, "binop_EQ_bCurrentCounter_MAXVALUE");
	objectCall5(unit_binop_EQ_bCurrentCounter_MAXVALUE, Connect, 0, wire_wireout_bCurrentCounter, 0, 0, width);
	objectCall5(unit_binop_EQ_bCurrentCounter_MAXVALUE, Connect, 1, wire_bConst_MAXVALUE, 0, 0, width);
	objectCall5(wire_wEQ_bCurrentCounter_MAXVALUE, Connect, 0, unit_binop_EQ_bCurrentCounter_MAXVALUE, 2, 0, 1);

	/* bCurrentCounter+1 �üӷ���ʵ�� */
	  /* ���� 1 */
	sprintf(temp, "%d", width);
	IHDL4SEUnit** wire_bConst_One = hdl4seCreateUnit(module_counter, CLSID_HDL4SE_WIRE, temp, "bConst_One");
	sprintf(temp, "%d, %d", width, 1);
	IHDL4SEUnit** unit_const_One = hdl4seCreateUnit(module_counter, CLSID_HDL4SE_CONST, temp, "const_One");
	objectCall5(wire_bConst_One, Connect, 0, unit_const_One, 0, 0, width);

	/* bCurrentCounter + 1 */
	sprintf(temp, "%d", width);
	IHDL4SEUnit** wire_bCurrentCounterPlusOne = hdl4seCreateUnit(module_counter, CLSID_HDL4SE_WIRE, temp, "bCurrentCounterPlusOne");
	sprintf(temp, "%d, %d, 1, %d", width, width, BINOP_ADD);
	IHDL4SEUnit** unit_binop_bCurrentCounterInc = hdl4seCreateUnit(module_counter, CLSID_HDL4SE_BINOP, temp, "binop_bCurrentCounterInc");
	objectCall5(unit_binop_bCurrentCounterInc, Connect, 0, wire_wireout_bCurrentCounter, 0, 0, width);
	objectCall5(unit_binop_bCurrentCounterInc, Connect, 1, wire_bConst_One, 0, 0, width);
	objectCall5(wire_bCurrentCounterPlusOne, Connect, 0, unit_binop_bCurrentCounterInc, 2, 0, width);

	/* if���������ѡ����ʵ�� */
	sprintf(temp, "%d", width * 2);
	IHDL4SEUnit** wire_bMuxIn = hdl4seCreateUnit(module_counter, CLSID_HDL4SE_WIRE, temp, "bMuxIn");

	sprintf(temp, "%d, %d", width, width);
	IHDL4SEUnit** unit_bind2to1 = hdl4seCreateUnit(module_counter, CLSID_HDL4SE_BINDING2TO1, temp, "bMuxIn");
	objectCall5(unit_bind2to1, Connect, 0, wire_bCurrentCounterPlusOne, 0, 0, width);
	objectCall5(unit_bind2to1, Connect, 1, wire_bConst_RESETVALUE, 0, 0, width);
	objectCall5(wire_bMuxIn, Connect, 0, unit_bind2to1, 2, 0, width);

	sprintf(temp, "%d, %d, %d", 1, 2 * width, width);
	IHDL4SEUnit** unit_muxCurrentCouter = hdl4seCreateUnit(module_counter, CLSID_HDL4SE_MUX, temp, "muxCurrentCouter");
	objectCall5(unit_muxCurrentCouter, Connect, 0, wire_wEQ_bCurrentCounter_MAXVALUE, 0, 0, 1);
	objectCall5(unit_muxCurrentCouter, Connect, 1, wire_bMuxIn, 0, 0, width * 2);
	objectCall5(wire_wireout_bCurrentCounter, Connect, 0, unit_muxCurrentCouter, 2, 0, width);

	IHDL4SEUnit** wire_bConst_10 = hdl4seCreateUnit(module_counter, CLSID_HDL4SE_WIRE, "2", "bConst_10");
	IHDL4SEUnit** unit_const_10 = hdl4seCreateUnit(module_counter, CLSID_HDL4SE_CONST, "2, 2", "const_10");
	objectCall5(wire_bConst_10, Connect, 0, unit_const_10, 0, 0, 2);

	IHDL4SEUnit** unit_muxOverflow = hdl4seCreateUnit(module_counter, CLSID_HDL4SE_MUX, "1,2,1", "muxOverflow");
	objectCall5(unit_muxOverflow, Connect, 0, wire_wEQ_bCurrentCounter_MAXVALUE, 0, 0, 1);
	objectCall5(unit_muxOverflow, Connect, 1, wire_bConst_10, 0, 0, 2);
	objectCall5(wire_wirein_wOverflow, Connect, 0, unit_muxOverflow, 2, 0, 1);

	/*�ͷ�module�ӿ�*/
	objectRelease(module_counter);
	/*����unit�ӿ�*/
	return unit_counter;
}
