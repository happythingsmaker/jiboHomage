template <class T>
class Queue {
    T* data;
    uint8_t mSize;
    uint8_t iterator = 1;
    T maxStddivValue;
	T minStddivValue;
    uint16_t mStddiv;
	uint32_t tempSum;

  public:
    Queue(uint8_t pSize) {
      //data = new T[pSize];
	  data = (T*)malloc(pSize * sizeof(T));
      mSize = pSize;
    }
    void push(T input) {
      if (iterator == mSize ) {
        iterator = 0;
      }
      data[iterator] = input;
      iterator ++;
    }

    uint32_t sum() {
	  tempSum = 0;
      for ( uint8_t i = 0 ; i < mSize; i ++) {
        tempSum += data[i];
      }
      return tempSum;
    }

    T average() {
      return (uint32_t)(sum() / mSize);
    }

    T stddiv() {
	  uint32_t tempSum = average();
      uint32_t tempDiffSumPower = 0;
      for ( uint8_t i = 0 ; i < mSize; i ++) {
        tempDiffSumPower += ((tempSum - data[i]) * (tempSum - data[i]));
		//tempDiffSumPower += ((tempSum - data[i])^2);
      }
      T tempStddiv = sqrt(tempDiffSumPower / mSize);
      //if (tempStddiv > maxStddivValue) {
      //  maxStddivValue =  tempStddiv;
     // }
	  maxStddivValue = max(maxStddivValue, tempStddiv);
	  minStddivValue = min(minStddivValue, tempStddiv);
	  
      mStddiv = tempStddiv;
      return tempStddiv ;
    }

    T getStddiv() {
      return mStddiv ;
    }

    T getMaxStddiv() {
      return maxStddivValue;
    }
	
	T getMinStddiv() {
      return maxStddivValue;
    }
	
	void decreaseMaxMinStddiv() {
      if(maxStddivValue > 0){
		maxStddivValue *= 0.9;		
	  }
	  if(minStddivValue < 220){
		minStddivValue *= 1.1;		
	  }
	    
    }

};