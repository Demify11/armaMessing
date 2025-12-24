#pragma once

#define ADD_OLDOFFSET(klass, name, value) namespace klass { inline __int64 name = value; }

namespace Offsets {

	ADD_OLDOFFSET(ModBase, World, 0x21C5140);
		
	ADD_OLDOFFSET(Magazine, ammo1, 0x94);
	ADD_OLDOFFSET(Magazine, ammo2, 0x8c);
	
	ADD_OLDOFFSET(Weapon, WeaponStatePerson, 0xC40);
	ADD_OLDOFFSET(Weapon, WeaponIndex, 0x2c); //add it to C40, its not a pointer. b64
	ADD_OLDOFFSET(Weapon, WeaponArrayList, 0x120);


}