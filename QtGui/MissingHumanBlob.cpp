#include "missinghumanblob.h"

MissingHumanBlob::MissingHumanBlob()
{

}

MissingHumanBlob::~MissingHumanBlob()
{

}

MissingHumanBlob::MissingHumanBlob(HumanBlob superHumanBlob)
{
	humanBlob = superHumanBlob;
	missedTime = 1;
}