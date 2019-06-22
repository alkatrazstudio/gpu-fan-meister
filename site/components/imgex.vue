<template>
<v-img contain v-bind="$props">
    <v-layout slot="placeholder" fill-height align-center justify-center>
        <v-progress-circular indeterminate color="grey lighten-5" :size="progressSize"></v-progress-circular>
    </v-layout>
</v-img>
</template>

<script>
const MAX_PROGRESS_SIZE = 30
const PROGRESS_MARGIN = 5

export default {
    props: ['src', 'aspect-ratio', 'height', 'alt'],
    computed: {
        progressSize() {
            return Math.min((parseInt(this.height) || MAX_PROGRESS_SIZE + PROGRESS_MARGIN) - PROGRESS_MARGIN, MAX_PROGRESS_SIZE)
        }
    }
}
</script>
