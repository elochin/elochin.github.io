BEGIN{
}
/\"streams\"/{
		getline
		getline
		getline
		time = $2; gsub(",","",time); 
		if (time > 0) {
			getline
			getline
			getline
			val = $2; gsub(",","",val); 
			if (val > 0) {
				printf "%s %s \n", time, val
			}
			#getline
			#getline
			#val = $2; gsub(",","",val); printf "%s \n",val
		}
	}
