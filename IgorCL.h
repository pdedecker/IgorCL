/*
IgorCL - an XOP to use OpenCL in Igor Pro
Copyright(C) 2013-2017 Peter Dedecker

This program is free software : you can redistribute it and / or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.If not, see <http://www.gnu.org/licenses/>.

The developer(s) of this software hereby grants permission to link
this program with Igor Pro, developed by WaveMetrics Inc. (www.wavemetrics.com).
*/

// Custom error codes
#define OLD_IGOR 1 + FIRST_XOP_ERR
#define OPENCL_ERROR 2 + FIRST_XOP_ERR

/* Prototypes */
HOST_IMPORT int XOPMain(IORecHandle ioRecHandle);
