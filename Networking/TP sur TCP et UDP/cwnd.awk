BEGIN{
}
/\"streams\"/{
		getline
		getline
		getline
		val = $2; gsub(",","",val); 
		if (val > 0) {
			printf "%s ",val
			getline
			getline
			getline
			#val = $2; gsub(",","",val); printf "%s \n",val
			getline
			getline
			val = $2; gsub(",","",val); printf "%s \n",val
		}
	}
