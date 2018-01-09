#include "Envelope.h"


float Envelope::attackVal(float normTime)
{
	return normTime / attack;
}

float Envelope::decayVal(float normTime)
{
	if (normTime < decay) {
		return normTime * ((sustain - 1) / decay) + 1; //linear because I'm lazy
	}
	else {
		return sustain;
	}
}

//Constructor
Envelope::Envelope()
{

}



float Envelope::oscillate(float time)
{
	if (time < attack) {
		return attackVal(time);
	}
	else if (time >= attack) {
		return decayVal(time - attack);
	}
	//ignoring release unless we add that to notes
	return 0.0;
}
