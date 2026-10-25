#ifndef ENTITYUNIQUEID_H
#define ENTITYUNIQUEID_H


class EntityUniqueID {
    
public:
	long long id;
	EntityUniqueID(long long id) : id(id) {
	}

	operator long long() const {
		return this->id;
	}
};


#endif //ENTITYUNIQUEID_H
