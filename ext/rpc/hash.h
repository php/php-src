#ifndef HASH_H
#define HASH_H

static int zend_ts_hash_remove_key_or_index(TsHashTable *ht, char *arKey, uint nKeyLength, void **pData);

static int zend_ts_hash_remove_key_or_index(TsHashTable *ht, char *arKey, uint nKeyLength, void **pData)
{
	uint nIndex;
	uint h = nKeyLength;
	uint result;
	void **ppData;
	Bucket *p;

	tsrm_mutex_lock(ht->mx_writer);

	if (arKey) {
		result = zend_hash_find(TS_HASH(ht), arKey, nKeyLength, (void **) &ppData);
	} else {
		result = zend_hash_index_find(TS_HASH(ht), h, (void **) &ppData);
	}

	if (result == SUCCESS) {
		*pData = *ppData;

		if (arKey) {
			h = zend_inline_hash_func(arKey, nKeyLength);
		}

		nIndex = h & TS_HASH(ht)->nTableMask;

		p = TS_HASH(ht)->arBuckets[nIndex];
		while (p != NULL) {
			if ((p->h == h) && ((p->nKeyLength == 0) || /* Numeric index */
				((p->nKeyLength == nKeyLength) && (!memcmp(p->arKey, arKey, nKeyLength))))) {
				HANDLE_BLOCK_INTERRUPTIONS();
				if (p == TS_HASH(ht)->arBuckets[nIndex]) {
					TS_HASH(ht)->arBuckets[nIndex] = p->pNext;
				} else {
					p->pLast->pNext = p->pNext;
				}
				if (p->pNext) {
					p->pNext->pLast = p->pLast;
				}
				if (p->pListLast != NULL) {
					p->pListLast->pListNext = p->pListNext;
				} else { 
					/* Deleting the head of the list */
					TS_HASH(ht)->pListHead = p->pListNext;
				}
				if (p->pListNext != NULL) {
					p->pListNext->pListLast = p->pListLast;
				} else {
					TS_HASH(ht)->pListTail = p->pListLast;
				}
				if (TS_HASH(ht)->pInternalPointer == p) {
					TS_HASH(ht)->pInternalPointer = p->pListNext;
				}
				if (!p->pDataPtr) {
					pefree(p->pData, TS_HASH(ht)->persistent);
				}
				pefree(p, TS_HASH(ht)->persistent);
				HANDLE_UNBLOCK_INTERRUPTIONS();
				TS_HASH(ht)->nNumOfElements--;
				return SUCCESS;
			}
			p = p->pNext;
		}
	}
	tsrm_mutex_unlock(ht->mx_writer);

	return FAILURE;
}

#endif /* HASH_H */
