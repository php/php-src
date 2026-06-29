#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <string.h>

#include "gd.h"
#include "gdhelpers.h"

typedef struct gdImageMetadataProfile {
	char *key;
	unsigned char *data;
	size_t size;
	struct gdImageMetadataProfile *next;
} gdImageMetadataProfile;

struct gdImageMetadata {
	gdImageMetadataProfile *profiles;
	size_t profile_count;
	size_t total_size;
	size_t max_profile_size;
	size_t max_total_size;
};

static int gdMetadataKeyIsValid(const char *key) {
	return key != NULL && key[0] != '\0';
}

static char *gdMetadataStrdup(const char *src) {
	size_t len;
	char *dst;

	len = strlen(src);
	dst = (char *)gdMalloc(len + 1);
	if (dst == NULL) {
		return NULL;
	}
	memcpy(dst, src, len + 1);
	return dst;
}

static gdImageMetadataProfile *
gdMetadataFindProfile(const gdImageMetadata *metadata, const char *key) {
	gdImageMetadataProfile *profile;

	if (metadata == NULL || !gdMetadataKeyIsValid(key)) {
		return NULL;
	}

	profile = metadata->profiles;
	while (profile != NULL) {
		if (strcmp(profile->key, key) == 0) {
			return profile;
		}
		profile = profile->next;
	}

	return NULL;
}

static void gdMetadataFreeProfile(gdImageMetadataProfile *profile) {
	if (profile == NULL) {
		return;
	}
	if (profile->key != NULL) {
		gdFree(profile->key);
	}
	if (profile->data != NULL) {
		gdFree(profile->data);
	}
	gdFree(profile);
}

BGD_DECLARE(gdImageMetadata *) gdImageMetadataCreate(void) {
	gdImageMetadata *metadata;

	metadata = (gdImageMetadata *)gdCalloc(1, sizeof(gdImageMetadata));
	if (metadata == NULL) {
		return NULL;
	}

	metadata->max_profile_size = GD_METADATA_DEFAULT_MAX_PROFILE_SIZE;
	metadata->max_total_size = GD_METADATA_DEFAULT_MAX_TOTAL_SIZE;

	return metadata;
}

BGD_DECLARE(void) gdImageMetadataFree(gdImageMetadata *metadata) {
	if (metadata == NULL) {
		return;
	}

	gdImageMetadataReset(metadata);
	gdFree(metadata);
}

BGD_DECLARE(void) gdImageMetadataReset(gdImageMetadata *metadata) {
	gdImageMetadataProfile *profile;

	if (metadata == NULL) {
		return;
	}

	profile = metadata->profiles;
	while (profile != NULL) {
		gdImageMetadataProfile *next = profile->next;
		gdMetadataFreeProfile(profile);
		profile = next;
	}

	metadata->profiles = NULL;
	metadata->profile_count = 0;
	metadata->total_size = 0;
}

BGD_DECLARE(int)
gdImageMetadataSetLimits(gdImageMetadata *metadata, size_t max_profile_size,
						 size_t max_total_size) {
	if (metadata == NULL) {
		return GD_META_ERR_INVALID;
	}

	if (max_profile_size != 0 || max_total_size != 0) {
		gdImageMetadataProfile *profile;

		if (max_total_size != 0 && metadata->total_size > max_total_size) {
			return GD_META_ERR_LIMIT;
		}

		profile = metadata->profiles;
		while (profile != NULL) {
			if (max_profile_size != 0 && profile->size > max_profile_size) {
				return GD_META_ERR_LIMIT;
			}
			profile = profile->next;
		}
	}

	metadata->max_profile_size = max_profile_size;
	metadata->max_total_size = max_total_size;
	return GD_META_OK;
}

BGD_DECLARE(void)
gdImageMetadataGetLimits(const gdImageMetadata *metadata,
						 size_t *max_profile_size, size_t *max_total_size) {
	if (max_profile_size != NULL) {
		*max_profile_size = metadata != NULL ? metadata->max_profile_size : 0;
	}
	if (max_total_size != NULL) {
		*max_total_size = metadata != NULL ? metadata->max_total_size : 0;
	}
}

BGD_DECLARE(int)
gdImageMetadataSetProfile(gdImageMetadata *metadata, const char *key,
						  const unsigned char *data, size_t size) {
	gdImageMetadataProfile *profile;
	unsigned char *new_data = NULL;
	size_t old_size = 0;
	size_t new_total;

	if (metadata == NULL || !gdMetadataKeyIsValid(key) ||
		(data == NULL && size != 0)) {
		return GD_META_ERR_INVALID;
	}

	if (metadata->max_profile_size != 0 && size > metadata->max_profile_size) {
		return GD_META_ERR_LIMIT;
	}

	profile = gdMetadataFindProfile(metadata, key);
	if (profile != NULL) {
		old_size = profile->size;
	}

	if ((size_t)-1 - (metadata->total_size - old_size) < size) {
		return GD_META_ERR_LIMIT;
	}
	new_total = metadata->total_size - old_size + size;
	if (metadata->max_total_size != 0 && new_total > metadata->max_total_size) {
		return GD_META_ERR_LIMIT;
	}

	if (size != 0) {
		new_data = (unsigned char *)gdMalloc(size);
		if (new_data == NULL) {
			return GD_META_ERR_NOMEM;
		}
		memcpy(new_data, data, size);
	}

	if (profile == NULL) {
		profile = (gdImageMetadataProfile *)gdCalloc(
			1, sizeof(gdImageMetadataProfile));
		if (profile == NULL) {
			if (new_data != NULL) {
				gdFree(new_data);
			}
			return GD_META_ERR_NOMEM;
		}
		profile->key = gdMetadataStrdup(key);
		if (profile->key == NULL) {
			if (new_data != NULL) {
				gdFree(new_data);
			}
			gdFree(profile);
			return GD_META_ERR_NOMEM;
		}
		profile->next = metadata->profiles;
		metadata->profiles = profile;
		metadata->profile_count++;
	} else if (profile->data != NULL) {
		gdFree(profile->data);
	}

	profile->data = new_data;
	profile->size = size;
	metadata->total_size = new_total;

	return GD_META_OK;
}

BGD_DECLARE(const unsigned char *)
gdImageMetadataGetProfile(const gdImageMetadata *metadata, const char *key,
						  size_t *size) {
	gdImageMetadataProfile *profile;

	if (size != NULL) {
		*size = 0;
	}

	profile = gdMetadataFindProfile(metadata, key);
	if (profile == NULL) {
		return NULL;
	}

	if (size != NULL) {
		*size = profile->size;
	}
	return profile->data;
}

BGD_DECLARE(int)
gdImageMetadataRemoveProfile(gdImageMetadata *metadata, const char *key) {
	gdImageMetadataProfile *profile;
	gdImageMetadataProfile *previous = NULL;

	if (metadata == NULL || !gdMetadataKeyIsValid(key)) {
		return GD_META_ERR_INVALID;
	}

	profile = metadata->profiles;
	while (profile != NULL) {
		if (strcmp(profile->key, key) == 0) {
			if (previous == NULL) {
				metadata->profiles = profile->next;
			} else {
				previous->next = profile->next;
			}
			metadata->profile_count--;
			metadata->total_size -= profile->size;
			gdMetadataFreeProfile(profile);
			return GD_META_OK;
		}
		previous = profile;
		profile = profile->next;
	}

	return GD_META_OK;
}

BGD_DECLARE(size_t)
gdImageMetadataGetProfileCount(const gdImageMetadata *metadata) {
	return metadata != NULL ? metadata->profile_count : 0;
}

BGD_DECLARE(int)
gdImageMetadataGetProfileAt(const gdImageMetadata *metadata, size_t index,
							const char **key, const unsigned char **data,
							size_t *size) {
	gdImageMetadataProfile *profile;
	size_t i = 0;

	if (key != NULL) {
		*key = NULL;
	}
	if (data != NULL) {
		*data = NULL;
	}
	if (size != NULL) {
		*size = 0;
	}

	if (metadata == NULL) {
		return GD_META_ERR_INVALID;
	}

	profile = metadata->profiles;
	while (profile != NULL) {
		if (i == index) {
			if (key != NULL) {
				*key = profile->key;
			}
			if (data != NULL) {
				*data = profile->data;
			}
			if (size != NULL) {
				*size = profile->size;
			}
			return GD_META_OK;
		}
		i++;
		profile = profile->next;
	}

	return GD_META_ERR_INVALID;
}
