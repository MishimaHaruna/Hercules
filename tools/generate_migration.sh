#!/usr/bin/env bash
#
# This file is part of Hercules.
# http://herc.ws - http://github.com/HerculesWS/Hercules
#
# Copyright (C) 2019  Hercules Dev Team
# Copyright (C) 2018  Haru
#
# Hercules is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

if [ -z "$1" ]; then
	echo "Usage: $0 <path to the sql-files folder>."
	exit -1
fi

SQLDIR="$1"
MIGRATIONDIR="$SQLDIR/upgrades"

if [ ! -d "$SQLDIR" ]; then
	echo "Error: The sql files folder $SQLDIR is missing."
	exit -1
fi

if [ ! -d "$MIGRATIONDIR" -o ! -f "$MIGRATIONDIR/index.txt" ]; then
	echo "Error: The migrations folder $MIGRATIONDIR is missing or invalid."
	exit -1
fi

date -j >/dev/null 2>&1
USING_GNU_COREUTILS=$?
SERIAL="$(date +%s)"
if [ $USING_GNU_COREUTILS -ne "0" ]; then
	FILENAME="$(date -d@$SERIAL -u +%Y-%m-%d--%H-%M)"
else
	FILENAME="$(date -j -u -f "%s" $SERIAL "+%Y-%m-%d--%H-%M")"
fi


if [ -f "$MIGRATIONDIR/$FILENAME.sql" ]; then
	echo "Error: A migration with the same name already exists. Please try again later."
	exit -1
fi

cat > "$MIGRATIONDIR/$FILENAME.sql" << EOF
#$SERIAL

-- This file is part of Hercules.
-- http://herc.ws - http://github.com/HerculesWS/Hercules
--
-- Copyright (C) $(date +%Y)  Hercules Dev Team
--
-- Hercules is free software: you can redistribute it and/or modify
-- it under the terms of the GNU General Public License as published by
-- the Free Software Foundation, either version 3 of the License, or
-- (at your option) any later version.
--
-- This program is distributed in the hope that it will be useful,
-- but WITHOUT ANY WARRANTY; without even the implied warranty of
-- MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
-- GNU General Public License for more details.
--
-- You should have received a copy of the GNU General Public License
-- along with this program.  If not, see <http://www.gnu.org/licenses/>.

-- #### INSERT MIGRATION HERE ####

INSERT INTO \`sql_updates\` (\`timestamp\`) VALUES ($SERIAL);
EOF

echo "$FILENAME.sql" >> "$MIGRATIONDIR/index.txt"

echo "Migration created as $MIGRATIONDIR/$FILENAME.sql"
echo "Insert the following line in your $SQLDIR/main.sql"
echo "INSERT IGNORE INTO \`sql_updates\` (\`timestamp\`) VALUES ($SERIAL); -- $FILENAME.sql"
