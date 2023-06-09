import {RES_VACANT, RES_BLUE, RES_GREEN, RES_RED, RES_YELLOW, SIDE_NONE, SIDE_1, SIDE_2, COUNT_RES} from '../core/constants.js';

export const SIZE_TILE = 100;
export const SIZE_TILE_DOT = 10;
export const SIZE_NODE = 15;
export const SIZE_ROAD_EMPTY_W = 55;
export const SIZE_ROAD_EMPTY_H = 15;
export const SIZE_ROAD_W = 55;
export const SIZE_ROAD_H = 10;
export const SIZE_TRADE_W = 100;
export const SIZE_TRADE_H = 50;


export const UNIT_TILE = SIZE_TILE/2; //More margin can be added, which adds a nice gutter, but also creates a bit of an optical illusion that makes it hard to look at
export const UNIT_NODE = SIZE_TILE/2;
export const UNIT_ROAD = SIZE_TILE/2;
export const UNIT_TRADE = SIZE_TRADE_W+10;

export const COUNT_TRADE = 3;

export const RES_COLORS = new Array(COUNT_RES+1);
RES_COLORS[RES_VACANT] = '#333333';
RES_COLORS[RES_BLUE] = '#0000cc';
RES_COLORS[RES_GREEN] = '#00cc00';
RES_COLORS[RES_RED] = '#cc0000';
RES_COLORS[RES_YELLOW] = '#cccc00';

export const RES_COLORS_HOVER = new Array(COUNT_RES+1);
RES_COLORS_HOVER[RES_VACANT] = '#aaaaaa';
RES_COLORS_HOVER[RES_BLUE] = '#0000aa';
RES_COLORS_HOVER[RES_GREEN] = '#00aa00';
RES_COLORS_HOVER[RES_RED] = '#aa0000';
RES_COLORS_HOVER[RES_YELLOW] = '#aaaa00';

export const RES_COLORS_DISABLED = new Array(COUNT_RES+1);
RES_COLORS_DISABLED[RES_VACANT] = '#aaaaaa';
RES_COLORS_DISABLED[RES_BLUE] = '#000066';
RES_COLORS_DISABLED[RES_GREEN] = '#006600';
RES_COLORS_DISABLED[RES_RED] = '#660000';
RES_COLORS_DISABLED[RES_YELLOW] = '#666600';

export const SIDE_COLORS = new Array(3);
SIDE_COLORS[SIDE_1] = '#ffa500';
SIDE_COLORS[SIDE_2] = '#9e7bb5';
SIDE_COLORS[SIDE_NONE] = '#aaaaaa';
