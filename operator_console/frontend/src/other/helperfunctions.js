

export function getRightmostAndLeftOfLastDot(input) {
    // Handle empty or invalid input
    if (!input || typeof input !== 'string') {
        return { leftOfLast: '', rightmost: '' };
    }

    // Find the last occurrence of '.'
    const lastDotIndex = input.lastIndexOf('.');

    // Extract text before and after the last '.'
    const leftOfLast = lastDotIndex !== -1 ? input.substring(0, lastDotIndex) : input;
    const rightmost = lastDotIndex !== -1 ? input.substring(lastDotIndex + 1) : input;

    return { leftOfLast, rightmost };
}

export function transformFlatJsonToNested(flatJson) {
    // Handle invalid input
    if (!flatJson || typeof flatJson !== 'object') {
        return {};
    }

    const nestedJson = {};

    // Iterate over flat JSON entries
    for (const [key, value] of Object.entries(flatJson)) {
        const { leftOfLast: tag_name, rightmost: field } = getRightmostAndLeftOfLastDot(key);

        // Skip invalid keys
        if (!tag_name || !field) {
            continue;
        }

        // Initialize nested object for tag_name if it doesn't exist
        if (!nestedJson[tag_name]) {
            nestedJson[tag_name] = {};
        }

        // Add field and value to the nested object
        nestedJson[tag_name][field] = value;
    }

    return nestedJson;
}