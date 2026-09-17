/*
 * This program source code file is part of WiCAD, a fork of KiCad.
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <pcad_mode.h>
#include <pgm_base.h>
#include <settings/common_settings.h>


bool IsPCadMode()
{
    if( PGM_BASE* pgm = PgmOrNull() )
    {
        if( COMMON_SETTINGS* settings = pgm->GetCommonSettings() )
            return settings->m_PCadMode.enabled;
    }

    return true;
}
